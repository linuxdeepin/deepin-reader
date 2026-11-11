// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * htmltopdf 模块单元测试（白盒）
 *
 * 通过 #include "htmltopdfconverter.cpp" 覆盖匿名命名空间内的辅助类：
 *   - HiDpiImageHandler      显示尺寸与嵌入分辨率分离的图片 handler
 *   - EmbeddingTextDocument  300dpi 嵌入上限的资源缩放
 *   - preloadImageResources  打印 clone 前的资源预加载
 *   - referenceDpi/layoutDpiScale  打印 DPI 换算
 * htmltopdfconverter.cpp 尾部 #include 的 "htmltopdfconverter.moc" 由
 * tests/CMakeLists.txt 中的自定义 moc 命令生成（与主程序 AUTOMOC 等价）。
 */

#ifdef UT_HTMLTOPDF_ENABLED

#include "htmltopdfconverter.cpp"

#include <gtest/gtest.h>

#include <QFile>
#include <QFileInfo>
#include <QGuiApplication>
#include <QImage>
#include <QPainter>
#include <QPrinter>
#include <QRegularExpression>
#include <QScreen>
#include <QTemporaryDir>
#include <QTextImageFormat>
#include <QThread>
#include <QUrl>

namespace {

constexpr int kImgWidth = 800;
constexpr int kImgHeight = 400;
// 图片特征色，用于 drawObject 渲染像素断言
const QColor kImageColor(30, 90, 180);

QString writePng(const QString &path, int width, int height, const QColor &color)
{
    QImage image(width, height, QImage::Format_RGB32);
    image.fill(color);
    QPainter painter(&image);
    painter.setPen(Qt::black);
    painter.drawRect(0, 0, width - 1, height - 1);
    painter.end();
    if (!image.save(path))
        return QString();
    return path;
}

QTextImageFormat makeImageFormat(const QString &filePath, qreal width = -1, qreal height = -1)
{
    QTextImageFormat format;
    format.setName(filePath);
    if (width > 0)
        format.setWidth(width);
    if (height > 0)
        format.setHeight(height);
    return format;
}

}  // namespace

// ====== DPI 换算 ======

TEST(HtmlTopdfDpiTest, ReferenceDpi_MatchesPrimaryScreenLogicalDpi)
{
    if (QGuiApplication::primaryScreen())
        EXPECT_EQ(referenceDpi(), QGuiApplication::primaryScreen()->logicalDotsPerInchY());
    else
        EXPECT_EQ(referenceDpi(), 96.0);
}

TEST(HtmlTopdfDpiTest, LayoutDpiScale_WithoutPaintDevice_ReturnsOne)
{
    QTextDocument doc;
    EXPECT_DOUBLE_EQ(layoutDpiScale(&doc), 1.0);
}

TEST(HtmlTopdfDpiTest, LayoutDpiScale_WithPrinterPaintDevice_ScalesByDeviceDpi)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);

    QTextDocument doc;
    doc.documentLayout()->setPaintDevice(&printer);

    EXPECT_NEAR(layoutDpiScale(&doc),
                qreal(printer.logicalDpiY()) / referenceDpi(), 1e-9);
}

// ====== HtmltoPdfConverter 公开 API ======

class HtmlTopdfConvertTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        ASSERT_TRUE(m_dir.isValid());
        m_imgPath = writePng(m_dir.filePath("img.png"), kImgWidth, kImgHeight, kImageColor);
        ASSERT_FALSE(m_imgPath.isEmpty());
    }

    QString writeHtml(const QString &imgSrc)
    {
        const QString content = QStringLiteral(
            "<!DOCTYPE html><html><head><meta charset=\"utf-8\"></head><body>"
            "<h1>标题测试</h1><p>中文正文段落，包含<b>粗体</b>与<i>斜体</i>。</p>"
            "<table><tr><th>列一</th><th>列二</th></tr><tr><td>A</td><td>B</td></tr></table>"
            "<ul><li>条目一</li><li>条目二</li></ul>"
            "<img src=\"%1\" alt=\"test\"></body></html>").arg(imgSrc);
        const QString path = m_dir.filePath("input.html");
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return QString();
        file.write(content.toUtf8());
        return path;
    }

    QTemporaryDir m_dir;
    QString m_imgPath;
};

TEST_F(HtmlTopdfConvertTest, ValidHtml_ProducesNonEmptyPdf)
{
    const QString input = writeHtml(QFileInfo(m_imgPath).fileName());
    ASSERT_FALSE(input.isEmpty());
    const QString output = m_dir.filePath("out.pdf");

    EXPECT_EQ(HtmltoPdfConverter(input, output).run(), 0);

    QFile pdf(output);
    ASSERT_TRUE(pdf.open(QIODevice::ReadOnly));
    EXPECT_GT(pdf.size(), 0);
    EXPECT_EQ(pdf.read(5), QByteArray("%PDF-"));
}

TEST_F(HtmlTopdfConvertTest, MissingInputFile_ReturnsError)
{
    const QString output = m_dir.filePath("out.pdf");
    EXPECT_EQ(HtmltoPdfConverter(m_dir.filePath("no-such.html"), output).run(), 1);
    EXPECT_FALSE(QFile::exists(output));
}

TEST_F(HtmlTopdfConvertTest, UnwritableOutput_ReturnsError)
{
    const QString input = writeHtml(QFileInfo(m_imgPath).fileName());
    ASSERT_FALSE(input.isEmpty());
    const QString output = m_dir.filePath("no-such-dir/out.pdf");

    EXPECT_EQ(HtmltoPdfConverter(input, output).run(), 1);
    EXPECT_FALSE(QFile::exists(output));
}

TEST_F(HtmlTopdfConvertTest, OversizedImage_DisplayClampedAndEmbeddedUnderDpiCap)
{
    // 无显式尺寸的超大截图：显示宽度钳制到正文区，嵌入分辨率不超过 300dpi 上限
    writePng(m_dir.filePath("big.png"), 3000, 2000, kImageColor);
    const QString input = writeHtml("big.png");
    ASSERT_FALSE(input.isEmpty());
    const QString output = m_dir.filePath("big.pdf");

    ASSERT_EQ(HtmltoPdfConverter(input, output).run(), 0);

    QFile pdf(output);
    ASSERT_TRUE(pdf.open(QIODevice::ReadOnly));
    // 解析 PDF 内嵌图像对象宽度：3000px 源图应缩放至约 2242px
    // （A4 正文宽 190mm × 300dpi = 2244，含引擎取整余量）
    static const QRegularExpression re("/Width (\\d+)");
    QRegularExpressionMatchIterator it = re.globalMatch(QString::fromLatin1(pdf.readAll()));
    int maxEmbeddedWidth = 0;
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        maxEmbeddedWidth = qMax(maxEmbeddedWidth, match.captured(1).toInt());
    }
    EXPECT_GT(maxEmbeddedWidth, 0);
    EXPECT_GE(maxEmbeddedWidth, 2200);
    EXPECT_LE(maxEmbeddedWidth, 2260);
}

// ====== HiDpiImageHandler ======

class HtmlTopdfImageHandlerTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        ASSERT_TRUE(m_dir.isValid());
        m_imgPath = writePng(m_dir.filePath("img.png"), kImgWidth, kImgHeight, kImageColor);
        ASSERT_FALSE(m_imgPath.isEmpty());
    }

    QTemporaryDir m_dir;
    QString m_imgPath;
};

TEST_F(HtmlTopdfImageHandlerTest, NoExplicitSize_LargeImage_ClampedToMaxWidth)
{
    QTextDocument doc;
    HiDpiImageHandler handler(300.0);

    const QSizeF size = handler.intrinsicSize(&doc, 0, makeImageFormat(m_imgPath));

    EXPECT_DOUBLE_EQ(size.width(), 300.0);
    EXPECT_DOUBLE_EQ(size.height(), 150.0);  // 800x400 等比缩小
}

TEST_F(HtmlTopdfImageHandlerTest, NoExplicitSize_SmallImage_KeepsSize)
{
    const QString small = writePng(m_dir.filePath("small.png"), 200, 100, kImageColor);
    ASSERT_FALSE(small.isEmpty());

    QTextDocument doc;
    HiDpiImageHandler handler(300.0);

    const QSizeF size = handler.intrinsicSize(&doc, 0, makeImageFormat(small));

    EXPECT_DOUBLE_EQ(size.width(), 200.0);
    EXPECT_DOUBLE_EQ(size.height(), 100.0);
}

TEST_F(HtmlTopdfImageHandlerTest, NoExplicitSize_WithPrintPaintDevice_AppliesDpiScale)
{
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);

    QTextDocument doc;
    doc.documentLayout()->setPaintDevice(&printer);

    HiDpiImageHandler handler(300.0);
    const QSizeF size = handler.intrinsicSize(&doc, 0, makeImageFormat(m_imgPath));

    // 与 Qt 默认 QTextImageHandler 相同的 DPI 参考系（与文本保持一致）
    const qreal scale = qreal(printer.logicalDpiY()) / referenceDpi();
    EXPECT_NEAR(size.width(), 300.0 * scale, 1.0);
    EXPECT_NEAR(size.height(), 150.0 * scale, 1.0);
}

TEST_F(HtmlTopdfImageHandlerTest, ExplicitWidthOnly_CompletesHeightByAspect)
{
    QTextDocument doc;
    HiDpiImageHandler handler(300.0);

    const QSizeF size = handler.intrinsicSize(&doc, 0, makeImageFormat(m_imgPath, 200));

    EXPECT_DOUBLE_EQ(size.width(), 200.0);
    EXPECT_DOUBLE_EQ(size.height(), 100.0);
}

TEST_F(HtmlTopdfImageHandlerTest, ExplicitHeightOnly_CompletesWidthByAspect)
{
    QTextDocument doc;
    HiDpiImageHandler handler(300.0);

    const QSizeF size = handler.intrinsicSize(&doc, 0, makeImageFormat(m_imgPath, -1, 100));

    EXPECT_DOUBLE_EQ(size.width(), 200.0);
    EXPECT_DOUBLE_EQ(size.height(), 100.0);
}

TEST_F(HtmlTopdfImageHandlerTest, ExplicitBoth_UsesGivenValues)
{
    QTextDocument doc;
    HiDpiImageHandler handler(300.0);

    const QSizeF size = handler.intrinsicSize(&doc, 0, makeImageFormat(m_imgPath, 200, 50));

    EXPECT_DOUBLE_EQ(size.width(), 200.0);
    EXPECT_DOUBLE_EQ(size.height(), 50.0);
}

TEST_F(HtmlTopdfImageHandlerTest, MissingImage_ReturnsEmptySize)
{
    QTextDocument doc;
    HiDpiImageHandler handler(300.0);

    const QSizeF size = handler.intrinsicSize(&doc, 0, makeImageFormat(m_dir.filePath("ghost.png")));

    EXPECT_TRUE(size.isEmpty());
}

TEST_F(HtmlTopdfImageHandlerTest, QrcPath_MissingResource_ReturnsEmptySize)
{
    QTextDocument doc;
    HiDpiImageHandler handler(300.0);

    // ":/" 前缀资源走 qrc 分支
    const QSizeF size = handler.intrinsicSize(&doc, 0, makeImageFormat(":/no/such/image.png"));

    EXPECT_TRUE(size.isEmpty());
}

TEST_F(HtmlTopdfImageHandlerTest, DrawObject_RendersImagePixels)
{
    QTextDocument doc;
    HiDpiImageHandler handler(300.0);

    QImage surface(400, 200, QImage::Format_RGB32);
    surface.fill(Qt::white);
    QPainter painter(&surface);
    handler.drawObject(&painter, QRectF(0, 0, 400, 200), &doc, 0, makeImageFormat(m_imgPath));
    painter.end();

    // 源图整体为特征色，缩放绘制后中心像素应保持同色
    EXPECT_EQ(surface.pixelColor(200, 100), kImageColor);
}

// ====== EmbeddingTextDocument（300dpi 嵌入上限） ======

class HtmlTopdfEmbeddingTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        ASSERT_TRUE(m_dir.isValid());
        m_bigPath = writePng(m_dir.filePath("big.png"), 3000, 2000, kImageColor);
        ASSERT_FALSE(m_bigPath.isEmpty());
        m_smallPath = writePng(m_dir.filePath("small.png"), 800, 400, kImageColor);
        ASSERT_FALSE(m_smallPath.isEmpty());
    }

    QTemporaryDir m_dir;
    QString m_bigPath;
    QString m_smallPath;
};

TEST_F(HtmlTopdfEmbeddingTest, LoadResource_SmallImage_ReturnsOriginal)
{
    EmbeddingTextDocument doc(1000.0);

    const QImage image = doc.loadResource(QTextDocument::ImageResource,
                                          QUrl(m_smallPath)).value<QImage>();

    EXPECT_EQ(image.width(), 800);
    EXPECT_EQ(image.height(), 400);
}

TEST_F(HtmlTopdfEmbeddingTest, LoadResource_OversizedImage_ScalesToCapWidth)
{
    EmbeddingTextDocument doc(1000.0);

    const QImage image = doc.loadResource(QTextDocument::ImageResource,
                                          QUrl(m_bigPath)).value<QImage>();

    EXPECT_EQ(image.width(), 1000);
}

TEST_F(HtmlTopdfEmbeddingTest, LoadResource_KeepsAspectRatio)
{
    EmbeddingTextDocument doc(1000.0);

    const QImage image = doc.loadResource(QTextDocument::ImageResource,
                                          QUrl(m_bigPath)).value<QImage>();

    EXPECT_EQ(image.height(), 667);  // 2000 * 1000/3000 ≈ 666.7 → 667
}

TEST_F(HtmlTopdfEmbeddingTest, LoadResource_MissingFile_ReturnsNullImage)
{
    EmbeddingTextDocument doc(1000.0);

    const QImage image = doc.loadResource(QTextDocument::ImageResource,
                                          QUrl(m_dir.filePath("ghost.png"))).value<QImage>();

    EXPECT_TRUE(image.isNull());
}

// ====== preloadImageResources（打印 clone 的资源预加载） ======

class HtmlTopdfPreloadTest : public ::testing::Test {
protected:
    void SetUp() override
    {
        ASSERT_TRUE(m_dir.isValid());
        m_imgPath = writePng(m_dir.filePath("img.png"), kImgWidth, kImgHeight, kImageColor);
        ASSERT_FALSE(m_imgPath.isEmpty());
    }

    QTemporaryDir m_dir;
    QString m_imgPath;
};

TEST_F(HtmlTopdfPreloadTest, CachesImages_AndSurvivesDocumentClone)
{
    EmbeddingTextDocument doc(2242.0);
    doc.setBaseUrl(QUrl::fromLocalFile(m_dir.path() + QLatin1Char('/')));
    doc.setHtml(QStringLiteral("<p>前文</p><img src=\"img.png\" alt=\"a\"><p>后文</p>"));

    preloadImageResources(&doc);

    // print() 内部的 clone 会复制 resources 缓存并清空 cachedResources，
    // clone 后仍命中说明预加载写入了正确的缓存表
    QTextDocument *clone = doc.clone();
    ASSERT_NE(clone, nullptr);
    const QImage image = clone->resource(QTextDocument::ImageResource,
                                         QUrl("img.png")).value<QImage>();
    EXPECT_EQ(image.width(), kImgWidth);
    delete clone;
}

TEST_F(HtmlTopdfPreloadTest, EmbeddingCap_PropagatesThroughClone)
{
    writePng(m_dir.filePath("big.png"), 3000, 2000, kImageColor);

    EmbeddingTextDocument doc(1000.0);
    doc.setBaseUrl(QUrl::fromLocalFile(m_dir.path() + QLatin1Char('/')));
    doc.setHtml(QStringLiteral("<img src=\"big.png\" alt=\"b\">"));

    preloadImageResources(&doc);

    // 300dpi 上限缩放后的图片经缓存传入 clone —— print() 流程的关键行为
    QTextDocument *clone = doc.clone();
    ASSERT_NE(clone, nullptr);
    const QImage image = clone->resource(QTextDocument::ImageResource,
                                         QUrl("big.png")).value<QImage>();
    EXPECT_EQ(image.width(), 1000);
    delete clone;
}

TEST_F(HtmlTopdfPreloadTest, DocumentWithoutImages_NoCrash)
{
    QTextDocument doc;
    doc.setHtml(QStringLiteral("<p>纯文本段落</p>"));

    preloadImageResources(&doc);

    SUCCEED();
}

// ---- 临时诊断（排查后删除） ----

#endif  // UT_HTMLTOPDF_ENABLED

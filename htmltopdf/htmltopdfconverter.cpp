// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "htmltopdfconverter.h"

#include <QTextDocument>
#include <QTextBlock>
#include <QTextFragment>
#include <QAbstractTextDocumentLayout>
#include <QPrinter>
#include <QPageSize>
#include <QPageLayout>
#include <QMarginsF>
#include <QImage>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>
#include <QFile>
#include <QFileInfo>
#include <QUrl>
#include <QDebug>

#include "ddlog.h"

namespace {

// 嵌入图片的目标分辨率：300 dpi（打印质量）
constexpr qreal kTargetDpi = 300.0;
// CSS 逻辑分辨率（HTML 语义中 1 CSS px = 1/96 inch）
constexpr qreal kCssDpi = 96.0;
// QTextDocument::print() 会给 clone 文档额外设置 2cm 正文边距（96dpi 参考系下 75px，
// 叠加在 QPrinter 页边距之内），计算页面可用宽度时需扣除
constexpr qreal kPrintBodyMarginCssPx = 75.0;

/**
 * @brief 参考分辨率，与 Qt 内部 qt_defaultDpi() 语义一致（主屏逻辑 DPI）
 * QTextDocument 以屏幕像素为文档坐标基准，打印时整体按 paintDevice DPI 重缩放，
 * 图片对象必须使用同一参考系才能与文本保持一致的物理尺寸
 */
qreal referenceDpi()
{
    if (const QScreen *screen = QGuiApplication::primaryScreen())
        return screen->logicalDotsPerInchY();
    return kCssDpi;
}

/**
 * @brief 布局坐标系换算系数
 * QTextDocument::print() 在 paintDevice（PDF 引擎，1200dpi）像素坐标系下重新布局：
 * 文本随 paintDevice DPI 自动缩放，而自定义 object handler 的返回值按原始像素使用，
 * 须把 CSS 像素（1/96 inch）换算为 paintDevice 像素，否则打印输出中图片会缩小 DPI 倍
 */
qreal layoutDpiScale(const QTextDocument *doc)
{
    const QPaintDevice *paintDevice = doc->documentLayout()->paintDevice();
    if (!paintDevice)
        return 1.0;
    return qreal(paintDevice->logicalDpiY()) / referenceDpi();
}

/**
 * @brief 获取图片资源（与 QTextImageHandler 的资源解析逻辑一致）
 * QTextImageFormat::url() 在 Qt5.14 才引入，用 name() 保持 Qt5/Qt6 兼容
 */
inline QImage loadImageResource(QTextDocument *doc, const QTextImageFormat &imageFormat)
{
    QString name = imageFormat.name();
    if (name.startsWith(QLatin1String(":/"))) // qrc 资源无文件扩展名，需显式前缀
        name = QStringLiteral("qrc") + name;
    return doc->resource(QTextDocument::ImageResource, QUrl(name)).value<QImage>();
}

/**
 * @brief The HiDpiImageHandler class
 * 自定义图片 handler，替代 QTextDocumentLayout 默认的图片处理：
 * - 显示尺寸：不超过页面可用宽度（无显式宽度的超大图按页宽等比显示，避免溢出被裁剪）
 * - 嵌入像素：图片资源按原始（或 300dpi 上限）分辨率嵌入，显示尺寸与嵌入分辨率分离，
 *   高分辨率截图打印/放大查看不再模糊
 */
class HiDpiImageHandler : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)
public:
    explicit HiDpiImageHandler(qreal maxDisplayWidthCssPx, QObject *parent = nullptr)
        : QObject(parent), m_maxDisplayWidth(maxDisplayWidthCssPx) {}

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override
    {
        Q_UNUSED(posInDocument);
        const QTextImageFormat imageFormat = format.toImageFormat();
        const bool hasWidth = imageFormat.hasProperty(QTextFormat::ImageWidth);
        const bool hasHeight = imageFormat.hasProperty(QTextFormat::ImageHeight);

        // 显式指定了宽/高：与浏览器语义一致，按属性值显示（缺省维度按图片纵横比补全）
        if (hasWidth || hasHeight) {
            qreal width = imageFormat.width();
            qreal height = imageFormat.height();
            const QImage image = loadImageResource(doc, imageFormat);
            if (!image.isNull()) {
                if (hasWidth && !hasHeight && image.width() > 0)
                    height = width * qreal(image.height()) / qreal(image.width());
                else if (!hasWidth && hasHeight && image.height() > 0)
                    width = height * qreal(image.width()) / qreal(image.height());
            }
            return QSizeF(width, height) * layoutDpiScale(doc);
        }

        const QImage image = loadImageResource(doc, imageFormat);
        if (image.isNull())
            return QSizeF();

        qreal width = image.width();
        qreal height = image.height();
        if (width > m_maxDisplayWidth && width > 0) {
            height *= m_maxDisplayWidth / width;
            width = m_maxDisplayWidth;
        }
        return QSizeF(width, height) * layoutDpiScale(doc);
    }

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                    int posInDocument, const QTextFormat &format) override
    {
        Q_UNUSED(posInDocument);
        const QTextImageFormat imageFormat = format.toImageFormat();
        const QImage image = loadImageResource(doc, imageFormat);
        if (!image.isNull())
            painter->drawImage(rect, image);
    }

private:
    qreal m_maxDisplayWidth; // 页面可用宽度对应的 CSS 像素
};

/**
 * @brief The EmbeddingTextDocument class
 * 对超过 300dpi 打印质量对应宽度的超大图片做等比缩放：
 * 保留高分辨率嵌入（清晰度）的同时控制 PDF 体积；常规图片按原始分辨率嵌入。
 */
class EmbeddingTextDocument : public QTextDocument
{
public:
    explicit EmbeddingTextDocument(qreal maxEmbedWidthPx, QObject *parent = nullptr)
        : QTextDocument(parent), m_maxEmbedWidth(maxEmbedWidthPx) {}

    QVariant loadResource(int type, const QUrl &name) override
    {
        QVariant resource = QTextDocument::loadResource(type, name);
        if (type == QTextDocument::ImageResource) {
            const QImage image = resource.value<QImage>();
            if (!image.isNull() && image.width() > 0 && image.width() > m_maxEmbedWidth)
                return image.scaledToWidth(qRound(m_maxEmbedWidth), Qt::SmoothTransformation);
        }
        return resource;
    }

    /**
     * @brief 预加载图片资源：直调虚函数 loadResource，绕过 resource() 的缓存命中
     * QTextDocument::loadResource 是 protected，外部只能经 resource() 查询，而
     * resource() 会命中 cachedResources 里未经本类缩放的原图；预加载需要走完整
     * 加载链（含 300dpi 上限缩放），必须在成员函数内调用。
     */
    QVariant loadScaledResource(int type, const QUrl &name)
    {
        return loadResource(type, name);
    }

private:
    qreal m_maxEmbedWidth; // 300dpi 对应的嵌入像素上限
};

/**
 * @brief 预加载文档中的图片资源到资源缓存
 * QTextDocument::print() 会在内部 clone 的普通 QTextDocument 上完成布局与绘制，而
 * clone() 既不复制 baseUrl，也不携带 cachedResources，子类的 loadResource（300dpi
 * 上限缩放）在 clone 中不再被调用；若不预加载，print 的渲染只能靠资源加载的
 * 相对路径 fallback 兜底，与工作目录相关且丢失缩放控制。
 *
 * 这里经本类 loadResource 走完整加载链（含 300dpi 上限缩放），并把结果同时写入
 * “原样资源名”与“baseUrl 解析后”两个 key：原文档上的资源查询用解析后 key 命中，
 * print 内部 clone（无 baseUrl）用原样名命中，两个渲染路径都能拿到缩放后的图。
 */
void preloadImageResources(QTextDocument *document)
{
    auto *embeddingDoc = static_cast<EmbeddingTextDocument *>(document);
    for (QTextBlock block = embeddingDoc->begin(); block.isValid(); block = block.next()) {
        for (QTextBlock::iterator it = block.begin(); !it.atEnd(); ++it) {
            const QTextFragment fragment = it.fragment();
            if (!fragment.isValid() || !fragment.charFormat().isImageFormat())
                continue;
            const QTextImageFormat imageFormat = fragment.charFormat().toImageFormat();
            if (imageFormat.name().isEmpty())
                continue;

            const QUrl name(imageFormat.name());
            const QUrl resolved = embeddingDoc->baseUrl().isValid()
                                      ? embeddingDoc->baseUrl().resolved(name)
                                      : name;
            // 经本类 loadResource 直调绕过 resource() 的缓存查询：首次布局时基类会把原图
            // 塞进 cachedResources，缓存命中的返回值未经本类缩放，必须重新走加载链
            QVariant resource = embeddingDoc->loadScaledResource(QTextDocument::ImageResource, resolved);
            if (resource.isNull() && name != resolved)
                resource = embeddingDoc->loadScaledResource(QTextDocument::ImageResource, name);
            if (resource.isNull())
                continue;

            const QImage image = resource.value<QImage>();
            const QVariant variant = image.isNull() ? resource : QVariant::fromValue(image);
            embeddingDoc->addResource(QTextDocument::ImageResource, resolved, variant);
            if (name != resolved)
                embeddingDoc->addResource(QTextDocument::ImageResource, name, variant);
        }
    }
}

} // namespace

HtmltoPdfConverter::HtmltoPdfConverter(const QString &inputPath, const QString &outputPath)
    : m_inputPath(inputPath)
    , m_outputPath(outputPath)
{
    qCDebug(htmltopdfLog) << "HtmltoPdfConverter initialized with input:" << inputPath << "output:" << outputPath;
}

int HtmltoPdfConverter::run()
{
    qCDebug(htmltopdfLog) << "Starting HTML to PDF conversion";

    QFile inputFile(m_inputPath);
    if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCWarning(htmltopdfLog) << QString("Failed to open input file '%1': %2")
                          .arg(m_inputPath, inputFile.errorString());
        return 1;
    }
    const QString html = QString::fromUtf8(inputFile.readAll());
    inputFile.close();

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(m_outputPath);
    printer.setPageOrientation(QPageLayout::Portrait);
    printer.setPageSize(QPageSize(QPageSize::A4));
    // 10mm 边距，与浏览器默认打印边距(约0.4in)接近
    printer.setPageMargins(QMarginsF(10, 10, 10, 10), QPageLayout::Millimeter);

    // 页面可用宽度（英寸）
    const qreal pageWidthInch = printer.pageRect(QPrinter::DevicePixel).width()
                                / qMax(printer.logicalDpiX(), 1);
    // 显示尺寸上限（CSS px）：无显式宽度的超大图按正文区宽度显示，避免溢出
    const qreal maxDisplayWidthCssPx = qMax(pageWidthInch * kCssDpi - 2 * kPrintBodyMarginCssPx, 1.0);
    // 嵌入分辨率上限（像素）：按 300dpi 打印质量控制超大图的 PDF 体积
    const qreal maxEmbedWidthPx = pageWidthInch * kTargetDpi;

    EmbeddingTextDocument document(maxEmbedWidthPx);
    // 相对路径资源（图片等）以 html 所在目录为基准解析，与浏览器加载行为一致
    document.setBaseUrl(QUrl::fromLocalFile(QFileInfo(m_inputPath).absolutePath() + QLatin1Char('/')));
    document.setHtml(html);

    // 替换默认图片 handler：显示尺寸与嵌入分辨率分离，保证图片清晰
    HiDpiImageHandler *imageHandler = new HiDpiImageHandler(maxDisplayWidthCssPx, &document);
    document.documentLayout()->registerHandler(QTextFormat::ImageObject, imageHandler);

    // 预加载图片资源（经 300dpi 上限缩放后写入缓存），保证 print() 的 clone 布局同样生效
    preloadImageResources(&document);

    // QTextDocument::print 会按页面设置自动分页并输出矢量 PDF（字体自动嵌入）
    document.print(&printer);

    QFile outputFile(m_outputPath);
    if (!outputFile.exists() || outputFile.size() == 0) {
        qCWarning(htmltopdfLog) << QString("Failed to print to output file '%1'").arg(m_outputPath);
        return 1;
    }

    qCDebug(htmltopdfLog) << "HTML to PDF conversion completed:" << m_outputPath
                          << "size:" << outputFile.size();
    return 0;
}

#include "htmltopdfconverter.moc"

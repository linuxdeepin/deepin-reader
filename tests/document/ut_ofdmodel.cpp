// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "OfdModel.h"

#ifdef OFD_SUPPORT_ENABLED

#include "Model.h"
#include "ut_defines.h"

#include <QFile>
#include <QImage>
#include <QProcess>
#include <QTemporaryDir>

#include <algorithm>
#include <gtest/gtest.h>

using namespace deepin_reader;

namespace {

QString ofdFilePath()
{
    QString path = QString(UTSOURCEDIR) + "/files/normal.ofd";
    if (QFile(path).exists())
        return path;

    path = QCoreApplication::applicationDirPath() + "/files/normal.ofd";
    return path;
}

bool hasOfdFile()
{
    return QFile(ofdFilePath()).exists();
}

QString createOfdFixture(const QTemporaryDir &dir, const QByteArray &info,
                         const QByteArray &pageArea = QByteArray(),
                         const QByteArray &documentExtras = QByteArray())
{
    const QMap<QString, QByteArray> entries = {
        {"OFD.xml", "<OFD><DocBody><DocInfo>" + info
            + "</DocInfo><DocRoot>Document.xml</DocRoot></DocBody></OFD>"},
        {"Document.xml", "<Document><CommonData><PageArea><PhysicalBox>7 11 210 297</PhysicalBox>"
            "</PageArea></CommonData><Pages><Page ID=\"1\" BaseLoc=\"Page.xml\"/></Pages>"
            + documentExtras + "</Document>"},
        {"Page.xml", "<Page>" + pageArea + "<Content><Layer ID=\"2\"><PathObject ID=\"3\" "
            "Boundary=\"20 30 40 25\" Stroke=\"false\" Fill=\"true\"><FillColor Value=\"255 0 0\"/>"
            "<AbbreviatedData>M 0 0 L 40 0 L 40 25 L 0 25 C</AbbreviatedData>"
            "</PathObject></Layer></Content></Page>"}
    };
    for (auto it = entries.cbegin(); it != entries.cend(); ++it) {
        QFile file(dir.filePath(it.key()));
        if (!file.open(QIODevice::WriteOnly) || file.write(it.value()) != it.value().size())
            return {};
    }
    QProcess archive;
    archive.setWorkingDirectory(dir.path());
    archive.start(QStringLiteral("cmake"), {"-E", "tar", "cf", "fixture.ofd", "--format=zip",
                                           "OFD.xml", "Document.xml", "Page.xml"});
    if (!archive.waitForFinished() || archive.exitCode() != 0)
        return {};
    return dir.filePath("fixture.ofd");
}

} // namespace

class TestOfdModel : public ::testing::Test
{
public:
    void SetUp() override
    {
        if (!hasOfdFile())
            GTEST_SKIP() << "normal.ofd not available, skipping OFD model tests";

        Document::Error error = Document::NoError;
        m_doc.reset(OfdDocument::loadDocument(ofdFilePath(), error));
        ASSERT_NE(m_doc, nullptr);
        EXPECT_EQ(error, Document::NoError);
    }

    void TearDown() override {}

    QString m_path;
    std::unique_ptr<OfdDocument> m_doc;
};

TEST_F(TestOfdModel, loadDocument)
{
    EXPECT_GT(m_doc->pageCount(), 0);
}

TEST_F(TestOfdModel, pageSize)
{
    Page *page = m_doc->page(0);
    ASSERT_NE(page, nullptr);

    const QSizeF size = page->sizeF();
    EXPECT_GT(size.width(), 0.0);
    EXPECT_GT(size.height(), 0.0);

    delete page;
}

TEST_F(TestOfdModel, pageOutOfRange)
{
    EXPECT_EQ(m_doc->page(-1), nullptr);
    EXPECT_EQ(m_doc->page(m_doc->pageCount()), nullptr);
}

TEST_F(TestOfdModel, renderPage)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    const QImage image = page->render(400, 400);
    ASSERT_FALSE(image.isNull());
    EXPECT_GT(image.width(), 0);
    EXPECT_GT(image.height(), 0);

    // 渲染结果不应是纯白页面
    bool hasContentPixel = false;
    for (int y = 0; y < image.height() && !hasContentPixel; ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QRgb pixel = image.pixel(x, y);
            if (qRed(pixel) < 240 || qGreen(pixel) < 240 || qBlue(pixel) < 240) {
                hasContentPixel = true;
                break;
            }
        }
    }
    EXPECT_TRUE(hasContentPixel);
}

TEST_F(TestOfdModel, renderInvalidSize)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    EXPECT_TRUE(page->render(0, 0).isNull());
    EXPECT_TRUE(page->render(-10, 100).isNull());
}

TEST_F(TestOfdModel, renderSmallRegionOnHugeCanvas)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);
    // A full 42300 x 28000 image exceeds 4 GiB; only a 64 x 48 tile is needed.
    const QImage tile = page->render(42300, 28000, QRect(97, 42, 64, 48));
    ASSERT_FALSE(tile.isNull());
    EXPECT_EQ(tile.size(), QSize(64, 48));
}

TEST_F(TestOfdModel, rejectInvalidRegions)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);
    EXPECT_TRUE(page->render(423, 280, QRect(900, 0, 20, 20)).isNull());
    EXPECT_TRUE(page->render(423, 280, QRect(-1, 0, 20, 20)).isNull());
    EXPECT_TRUE(page->render(423, 280, QRect(420, 270, 20, 20)).isNull());
    EXPECT_TRUE(page->render(423, 280, QRect(1, 1, 0, 20)).isNull());
    EXPECT_TRUE(page->render(42300, 28000, QRect(0, 0, 16000, 16000)).isNull());
}

TEST(OfdApi, regionMatchesFullPageWithNonzeroPhysicalOrigin)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = createOfdFixture(dir, "<DocID>region</DocID>");
    ASSERT_FALSE(path.isEmpty());
    Document::Error error;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
    ASSERT_NE(doc, nullptr);
    std::unique_ptr<Page> page(doc->page(0));
    ASSERT_NE(page, nullptr);
    const QImage full = page->render(420, 594);
    ASSERT_FALSE(full.isNull());
    const QRect region(17, 19, 160, 150);
    const QImage tile = page->render(420, 594, region);
    ASSERT_FALSE(tile.isNull());
    EXPECT_EQ(tile, full.copy(region));
    EXPECT_EQ(tile.pixelColor(40, 40), QColor(Qt::red));

    const QImage largeCanvasTile = page->render(42000, 59400, QRect(4000, 5000, 64, 48));
    ASSERT_EQ(largeCanvasTile.size(), QSize(64, 48));
    EXPECT_EQ(largeCanvasTile.pixelColor(32, 24), QColor(Qt::red));
}

TEST_F(TestOfdModel, realInvoiceRegionMatchesFullPage)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);
    const QImage full = page->render(423, 280);
    const QRect region(97, 42, 129, 71);
    const QImage tile = page->render(423, 280, region);
    ASSERT_FALSE(full.isNull());
    ASSERT_EQ(tile.size(), region.size());
    int differingChannels = 0;
    int largestDelta = 0;
    for (int y = 0; y < tile.height(); ++y) {
        const uchar *expected = full.constScanLine(region.y() + y) + region.x() * 4;
        const uchar *actual = tile.constScanLine(y);
        for (int x = 0; x < tile.width() * 4; ++x) {
            const int delta = qAbs(int(expected[x]) - int(actual[x]));
            differingChannels += delta != 0;
            largestDelta = qMax(largestDelta, delta);
        }
    }
    // Cairo glyph/curve antialiasing may differ slightly with target extents.
    EXPECT_LE(largestDelta, 3);
    EXPECT_LE(differingChannels, 100);
}

TEST(OfdApi, metadataAndIdentifier)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = createOfdFixture(dir, QStringLiteral(
        "<DocID>文档-ID</DocID><Title>标题</Title><Author>作者</Author><Subject>主题</Subject>"
        "<Abstract>摘要</Abstract><Creator>应用</Creator><CreatorVersion>1.2</CreatorVersion>"
        "<CreationDate>2026-09-09</CreationDate><ModDate>2026-09-10T12:34:56+08:00</ModDate>"
        "<Keywords><Keyword>one</Keyword><Keyword>二</Keyword><Keyword>one</Keyword></Keywords>").toUtf8());
    ASSERT_FALSE(path.isEmpty());
    Document::Error error;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
    ASSERT_NE(doc, nullptr);
    const Properties props = doc->properties();
    EXPECT_EQ(doc->fileIdentifier(), QStringLiteral("文档-ID"));
    EXPECT_EQ(props.value("Title").toString(), QStringLiteral("标题"));
    EXPECT_EQ(props.value("Author").toString(), QStringLiteral("作者"));
    EXPECT_EQ(props.value("Subject").toString(), QStringLiteral("主题"));
    EXPECT_EQ(props.value("Description").toString(), QStringLiteral("摘要"));
    EXPECT_EQ(props.value("Creator").toString(), QStringLiteral("应用"));
    EXPECT_EQ(props.value("CreatorVersion").toString(), QStringLiteral("1.2"));
    EXPECT_EQ(props.value("KeyWords").toString(), QStringLiteral("one; 二; one"));
    EXPECT_EQ(props.value("CreationDate").toDateTime().date(), QDate(2026, 9, 9));
    EXPECT_EQ(props.value("ModificationDate").toDateTime().offsetFromUtc(), 8 * 3600);
    EXPECT_EQ(props.value("PageCount").toInt(), 1);
    EXPECT_EQ(doc->fileIdentifier(), QStringLiteral("文档-ID"));
}

TEST(OfdApi, missingMetadataAndInvalidDate)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = createOfdFixture(dir, "<CreationDate>not-a-date</CreationDate>");
    ASSERT_FALSE(path.isEmpty());
    Document::Error error;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
    ASSERT_NE(doc, nullptr);
    const Properties props = doc->properties();
    EXPECT_TRUE(doc->fileIdentifier().isEmpty());
    EXPECT_FALSE(props.contains("Title"));
    EXPECT_FALSE(props.value("CreationDate").toDateTime().isValid());
    EXPECT_EQ(props.value("CreationDateRaw").toString(), QStringLiteral("not-a-date"));
}

TEST(OfdApi, warningSnapshotRefreshesAfterLazyPageLoad)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = createOfdFixture(dir, "<DocID>warnings</DocID>");
    ASSERT_FALSE(path.isEmpty());
    Document::Error error;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
    ASSERT_NE(doc, nullptr);
    const QVariantList before = doc->properties().value("Warnings").toList();
    EXPECT_TRUE(before.isEmpty());
    std::unique_ptr<Page> page(doc->page(0));
    ASSERT_NE(page, nullptr);
    const QVariantList after = doc->properties().value("Warnings").toList();
    ASSERT_EQ(after.size(), 1);
    EXPECT_EQ(after.first().toMap().value("Code").toUInt(), ROFD_WARNING_PAGE_AREA_FALLBACK);
    EXPECT_EQ(after.first().toMap().value("Path").toString(), QStringLiteral("Page.xml"));
    EXPECT_FALSE(after.first().toMap().value("Message").toString().isEmpty());
    EXPECT_TRUE(before.isEmpty());
    std::unique_ptr<Page> again(doc->page(0));
    EXPECT_EQ(doc->properties().value("Warnings").toList(), after);
}

TEST(OfdApi, warningsRefreshAfterRenderingAnnotations)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = createOfdFixture(dir, "<DocID>annotation-warning</DocID>",
        "<Area><PhysicalBox>7 11 210 297</PhysicalBox></Area>",
        "<Annotations>missing.xml</Annotations>");
    ASSERT_FALSE(path.isEmpty());
    Document::Error error;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
    ASSERT_NE(doc, nullptr);
    std::unique_ptr<Page> page(doc->page(0));
    ASSERT_NE(page, nullptr);
    EXPECT_TRUE(doc->properties().value("Warnings").toList().isEmpty());
    ASSERT_FALSE(page->render(420, 594, QRect(17, 19, 160, 150)).isNull());
    const QVariantList warnings = doc->properties().value("Warnings").toList();
    ASSERT_EQ(warnings.size(), 1);
    EXPECT_EQ(warnings.first().toMap().value("Code").toUInt(), ROFD_WARNING_ANNOTATION_SKIPPED);
    EXPECT_EQ(warnings.first().toMap().value("Path").toString(), QStringLiteral("missing.xml"));
}

TEST_F(TestOfdModel, semanticFullText)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    const QString text = page->text(QRectF());
    EXPECT_FALSE(text.isEmpty());
    EXPECT_TRUE(text.contains(QStringLiteral("电子发票")));
}

TEST_F(TestOfdModel, semanticSearch)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    const QVector<PageSection> matches = page->search(QStringLiteral("电子发票"), true, false);
    ASSERT_FALSE(matches.isEmpty());
    ASSERT_FALSE(matches.first().isEmpty());
    EXPECT_TRUE(matches.first().first().rect.isValid());

    EXPECT_TRUE(page->search(QStringLiteral("不存在的文本"), false, false).isEmpty());
}

TEST_F(TestOfdModel, semanticWholeWordSearch)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    EXPECT_FALSE(page->search(QStringLiteral("电子"), false, false).isEmpty());
    EXPECT_TRUE(page->search(QStringLiteral("电子"), false, true).isEmpty());
}

TEST_F(TestOfdModel, semanticWordsAndAreaText)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    const QList<Word> words = page->words();
    ASSERT_FALSE(words.isEmpty());

    const auto character = std::find_if(words.cbegin(), words.cend(), [](const Word &word) {
        return word.text == QStringLiteral("电");
    });
    ASSERT_NE(character, words.cend());
    EXPECT_TRUE(character->boundingBox.isValid());
    EXPECT_GT(character->boundingBox.width(), 0.0);
    EXPECT_GT(character->boundingBox.height(), 0.0);

    const QRectF selection = character->boundingBox.adjusted(-0.1, -0.1, 0.1, 0.1);
    EXPECT_TRUE(page->text(selection).contains(character->text));
}

TEST_F(TestOfdModel, loadMissingFile)
{
    Document::Error error = Document::NoError;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(QStringLiteral("/nonexistent/path/to.ofd"), error));
    EXPECT_EQ(doc, nullptr);
    EXPECT_EQ(error, Document::FileError);
}

TEST_F(TestOfdModel, loadBrokenFile)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());

    const QString brokenPath = dir.filePath(QStringLiteral("broken.ofd"));
    QFile file(brokenPath);
    ASSERT_TRUE(file.open(QIODevice::WriteOnly));
    file.write("this is not an ofd zip archive");
    file.close();

    Document::Error error = Document::NoError;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(brokenPath, error));
    EXPECT_EQ(doc, nullptr);
    EXPECT_EQ(error, Document::FileError);
}

#endif // OFD_SUPPORT_ENABLED

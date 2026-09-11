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
#include <future>
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
                         const QByteArray &documentExtras = QByteArray(),
                         const QByteArray &secondPage = QByteArray())
{
    QMap<QString, QByteArray> entries = {
        {"OFD.xml", "<OFD><DocBody><DocInfo>" + info
            + "</DocInfo><DocRoot>Document.xml</DocRoot></DocBody></OFD>"},
        {"Document.xml", "<Document><CommonData><PageArea><PhysicalBox>7 11 210 297</PhysicalBox>"
            "</PageArea></CommonData><Pages><Page ID=\"1\" BaseLoc=\"Page.xml\"/>"
            + (secondPage.isEmpty() ? QByteArray() : QByteArray("<Page ID=\"42\" BaseLoc=\"Second.xml\"/>"))
            + "</Pages>"
            + documentExtras + "</Document>"},
        {"Page.xml", "<Page>" + pageArea + "<Content><Layer ID=\"2\"><PathObject ID=\"3\" "
            "Boundary=\"20 30 40 25\" Stroke=\"false\" Fill=\"true\"><FillColor Value=\"255 0 0\"/>"
            "<AbbreviatedData>M 0 0 L 40 0 L 40 25 L 0 25 C</AbbreviatedData>"
            "</PathObject></Layer></Content></Page>"}
    };
    if (!secondPage.isEmpty())
        entries.insert(QStringLiteral("Second.xml"), secondPage);
    for (auto it = entries.cbegin(); it != entries.cend(); ++it) {
        QFile file(dir.filePath(it.key()));
        if (!file.open(QIODevice::WriteOnly) || file.write(it.value()) != it.value().size())
            return {};
    }
    QProcess archive;
    archive.setWorkingDirectory(dir.path());
    QStringList arguments = {"-E", "tar", "cf", "fixture.ofd", "--format=zip"};
    arguments.append(entries.keys());
    archive.start(QStringLiteral("cmake"), arguments);
    if (!archive.waitForFinished() || archive.exitCode() != 0)
        return {};
    return dir.filePath("fixture.ofd");
}

const QByteArray explicitPageArea = "<Area><PhysicalBox>7 11 210 297</PhysicalBox></Area>";
const QByteArray offsetSecondPage = "<Page><Area><PhysicalBox>3 5 100 120</PhysicalBox></Area><Content/></Page>";

QByteArray outlineNode(const QByteArray &title, const QByteArray &action)
{
    return "<OutlineElem Title=\"" + title + "\"><Actions><Action Event=\"CLICK\">"
        + action + "</Action></Actions></OutlineElem>";
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

TEST(OfdApi, outlineKeepsNestedParentsAndCopiesNavigation)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    const QString path = createOfdFixture(dir, {}, explicitPageArea, R"xml(
        <Outlines><OutlineElem Title="目录" Expanded="false">
          <OutlineElem Title="无目标父节点" Expanded="true"><Actions>
            <Action Event="CLICK"><Goto><Dest Type="Fit" PageID="999"/></Goto></Action>
          </Actions><OutlineElem Title="书签"><Actions>
            <Action Event="DO"><URI URI="https://ignored.invalid/"/></Action>
            <Action Event="CLICK"><GotoA AttachID="attachment"/></Action>
            <Action Event="CLICK"><Sound/></Action>
            <Action Event="CLICK"><Goto><Bookmark Name="second"/></Goto></Action>
            <Action Event="CLICK"><URI URI="https://later.invalid/"/></Action>
          </Actions><OutlineElem Title="链接"><Actions>
            <Action Event="CLICK"><URI URI="child?q=1&amp;x=2" Base="https://example.invalid/base/"/></Action>
          </Actions></OutlineElem></OutlineElem></OutlineElem>
          <OutlineElem Title="末尾"/>
        </OutlineElem></Outlines>
        <Bookmarks><Bookmark Name="second"><Dest Type="XYZ" PageID="42" Left="13" Top="25" Zoom="0"/></Bookmark></Bookmarks>
    )xml", offsetSecondPage);
    ASSERT_FALSE(path.isEmpty());
    Document::Error error;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
    ASSERT_NE(doc, nullptr);
    const Outline outline = doc->outline();
    ASSERT_EQ(outline.size(), 1);
    const Section &root = outline.first();
    EXPECT_EQ(root.title, QStringLiteral("目录"));
    EXPECT_EQ(root.nIndex, -1);
    EXPECT_FALSE(root.navigation.has_value());
    ASSERT_TRUE(root.expanded.has_value());
    EXPECT_FALSE(*root.expanded);
    ASSERT_EQ(root.children.size(), 2);
    EXPECT_EQ(root.children.last().title, QStringLiteral("末尾"));
    const Section &parent = root.children.first();
    EXPECT_EQ(parent.nIndex, -1);
    EXPECT_FALSE(parent.navigation.has_value());
    ASSERT_TRUE(parent.expanded.has_value());
    EXPECT_TRUE(*parent.expanded);
    ASSERT_EQ(parent.children.size(), 1);
    const Section &bookmark = parent.children.first();
    ASSERT_TRUE(bookmark.navigation.has_value());
    ASSERT_TRUE(bookmark.navigation->destination.has_value());
    const NavigationDestination &destination = *bookmark.navigation->destination;
    EXPECT_EQ(destination.pageIndex, 1);
    EXPECT_EQ(bookmark.nIndex, 1);
    EXPECT_EQ(destination.mode, DestinationMode::XYZ);
    ASSERT_TRUE(destination.left.has_value());
    ASSERT_TRUE(destination.top.has_value());
    EXPECT_DOUBLE_EQ(*destination.left, (13.0 - 3.0) * doc->xRes() / 25.4);
    EXPECT_DOUBLE_EQ(*destination.top, (25.0 - 5.0) * doc->yRes() / 25.4);
    ASSERT_TRUE(destination.zoom.has_value());
    EXPECT_DOUBLE_EQ(*destination.zoom, 0.0);
    ASSERT_EQ(bookmark.children.size(), 1);
    const Section &uri = bookmark.children.first();
    ASSERT_TRUE(uri.navigation.has_value());
    EXPECT_EQ(uri.nIndex, -1);
    EXPECT_EQ(uri.navigation->uri, QUrl("https://example.invalid/base/child?q=1&x=2"));
    const QVariantList warnings = doc->properties().value("Warnings").toList();
    EXPECT_FALSE(warnings.isEmpty());
    EXPECT_EQ(doc->outline().first().children.first().children.first().title, bookmark.title);
    EXPECT_EQ(doc->properties().value("Warnings").toList(), warnings);
    std::unique_ptr<Page> page(doc->page(0));
    ASSERT_NE(page, nullptr);
    EXPECT_FALSE(page->render(210, 297).isNull());
    page.reset();
    doc.reset();
    EXPECT_EQ(root.title, QStringLiteral("目录"));
    EXPECT_EQ(uri.navigation->uri.host(), QStringLiteral("example.invalid"));
}

TEST(OfdApi, outlineDestinationModesKeepPresenceAndTargetOrigin)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    QByteArray nodes;
    const auto destNode = [](const QByteArray &mode, const QByteArray &fields) {
        return outlineNode(mode, "<Goto><Dest Type=\"" + mode + "\" PageID=\"42\" " + fields + "/></Goto>");
    };
    nodes += destNode("XYZ", "");
    nodes += destNode("XYZ", "Left=\"0\" Top=\"0\" Zoom=\"0\"");
    nodes += destNode("Fit", "");
    nodes += destNode("FitH", "Top=\"15\"");
    nodes += destNode("FitV", "Left=\"13\"");
    nodes += destNode("FitR", "Left=\"13\" Top=\"15\" Right=\"23\" Bottom=\"35\"");
    const QString path = createOfdFixture(dir, {}, explicitPageArea,
        "<Outlines>" + nodes + "</Outlines>", offsetSecondPage);
    ASSERT_FALSE(path.isEmpty());
    Document::Error error;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
    ASSERT_NE(doc, nullptr);
    const Outline outline = doc->outline();
    ASSERT_EQ(outline.size(), 6);
    const DestinationMode modes[] = {DestinationMode::XYZ, DestinationMode::XYZ, DestinationMode::Fit,
                                    DestinationMode::FitH, DestinationMode::FitV, DestinationMode::FitR};
    for (int i = 0; i < outline.size(); ++i) {
        ASSERT_TRUE(outline[i].navigation.has_value());
        ASSERT_TRUE(outline[i].navigation->destination.has_value());
        EXPECT_EQ(outline[i].navigation->destination->mode, modes[i]);
        EXPECT_EQ(outline[i].navigation->destination->pageIndex, 1);
        EXPECT_TRUE(outline[i].navigation->isValid());
    }
    const auto &absent = *outline[0].navigation->destination;
    EXPECT_FALSE(absent.left.has_value());
    EXPECT_FALSE(absent.top.has_value());
    EXPECT_FALSE(absent.zoom.has_value());
    const auto &zero = *outline[1].navigation->destination;
    ASSERT_TRUE(zero.left.has_value());
    ASSERT_TRUE(zero.top.has_value());
    ASSERT_TRUE(zero.zoom.has_value());
    EXPECT_DOUBLE_EQ(*zero.left, -3.0 * doc->xRes() / 25.4);
    EXPECT_DOUBLE_EQ(*zero.top, -5.0 * doc->yRes() / 25.4);
    EXPECT_DOUBLE_EQ(*zero.zoom, 0.0);
    const auto &fitH = *outline[3].navigation->destination;
    ASSERT_TRUE(fitH.top.has_value());
    EXPECT_DOUBLE_EQ(*fitH.top, 10.0 * doc->yRes() / 25.4);
    EXPECT_FALSE(fitH.left.has_value());
    const auto &fitV = *outline[4].navigation->destination;
    ASSERT_TRUE(fitV.left.has_value());
    EXPECT_DOUBLE_EQ(*fitV.left, 10.0 * doc->xRes() / 25.4);
    EXPECT_FALSE(fitV.top.has_value());
    const auto &fitR = *outline[5].navigation->destination;
    ASSERT_TRUE(fitR.right.has_value());
    ASSERT_TRUE(fitR.bottom.has_value());
    EXPECT_DOUBLE_EQ(*fitR.right, 20.0 * doc->xRes() / 25.4);
    EXPECT_DOUBLE_EQ(*fitR.bottom, 30.0 * doc->yRes() / 25.4);
}

TEST(OfdApi, outlineUnsafeAndInvalidActionsKeepNodesNonExecuting)
{
    QTemporaryDir dir;
    ASSERT_TRUE(dir.isValid());
    QByteArray nodes;
    const QList<QByteArray> actions = {
        "<Goto><Dest Type=\"Fit\" PageID=\"999\"/></Goto>",
        "<Goto><Bookmark Name=\"missing\"/></Goto>",
        "<Goto><Dest Type=\"CUSTOM\" PageID=\"42\"/></Goto>",
        "<Goto><Dest Type=\"FitR\" PageID=\"42\" Left=\"20\" Top=\"10\" Right=\"10\" Bottom=\"20\"/></Goto>",
        "<Goto><Dest Type=\"FitR\" PageID=\"42\" Left=\"0\"/></Goto>",
        "<URI URI=\"file:///tmp/example\"/>",
        "<URI URI=\"javascript:alert(1)\"/>",
        "<URI URI=\"relative\" Base=\"file:///tmp/\"/>",
        "<GotoA AttachID=\"attachment\"/>", "<Sound/>"
    };
    for (const QByteArray &action : actions)
        nodes += outlineNode("kept", action);
    nodes += "<OutlineElem Title=\"open-events\"><Actions>"
        "<Action Event=\"DO\"><Goto><Dest Type=\"Fit\" PageID=\"42\"/></Goto></Action>"
        "<Action Event=\"PO\"><URI URI=\"https://example.invalid/\"/></Action>"
        "</Actions><OutlineElem Title=\"child\"/></OutlineElem>";
    const QString path = createOfdFixture(dir, {}, explicitPageArea,
        "<Outlines>" + nodes + "</Outlines>", offsetSecondPage);
    ASSERT_FALSE(path.isEmpty());
    Document::Error error;
    std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
    ASSERT_NE(doc, nullptr);
    const Outline outline = doc->outline();
    ASSERT_EQ(outline.size(), actions.size() + 1);
    for (const Section &section : outline) {
        EXPECT_EQ(section.nIndex, -1);
        EXPECT_FALSE(section.navigation.has_value());
    }
    ASSERT_EQ(outline.last().children.size(), 1);
    EXPECT_EQ(outline.last().children.first().title, QStringLiteral("child"));
}

TEST(OfdApi, outlineGeometryQueriesRefreshWarningsAndKeepGoodPagesUsable)
{
    for (const QByteArray &secondPage : {QByteArray("<Page><Content/></Page>"), QByteArray("broken XML")}) {
        QTemporaryDir dir;
        ASSERT_TRUE(dir.isValid());
        const QString path = createOfdFixture(dir, {}, explicitPageArea,
            "<Outlines>" + outlineNode("target", "<Goto><Dest Type=\"XYZ\" PageID=\"42\" Left=\"17\"/></Goto>")
                + "</Outlines>", secondPage);
        ASSERT_FALSE(path.isEmpty());
        Document::Error error;
        std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
        ASSERT_NE(doc, nullptr);
        const QVariantList before = doc->properties().value("Warnings").toList();
        EXPECT_TRUE(before.isEmpty());
        const Outline outline = doc->outline();
        ASSERT_EQ(outline.size(), 1);
        if (secondPage.startsWith("<Page>")) {
            ASSERT_TRUE(outline.first().navigation.has_value());
            ASSERT_TRUE(outline.first().navigation->destination->left.has_value());
            EXPECT_DOUBLE_EQ(*outline.first().navigation->destination->left, 10.0 * doc->xRes() / 25.4);
            const QVariantList warnings = doc->properties().value("Warnings").toList();
            ASSERT_EQ(warnings.size(), 1);
            EXPECT_EQ(warnings.first().toMap().value("Code").toUInt(), ROFD_WARNING_PAGE_AREA_FALLBACK);
            EXPECT_EQ(warnings.first().toMap().value("Path").toString(), QStringLiteral("Second.xml"));
        } else {
            EXPECT_FALSE(outline.first().navigation.has_value());
            EXPECT_EQ(outline.first().nIndex, -1);
        }
        std::unique_ptr<Page> good(doc->page(0));
        ASSERT_NE(good, nullptr);
        EXPECT_FALSE(good->render(210, 297).isNull());
        EXPECT_EQ(doc->outline().size(), 1);
    }
}

TEST(OfdApi, outlineConcurrentAndEmptyOrFailedSnapshotsStayStable)
{
    const QByteArray deepOutline = "<Outlines>" + QByteArray("<OutlineElem Title=\"too deep\">").repeated(66)
        + QByteArray("</OutlineElem>").repeated(66) + "</Outlines>";
    for (const QByteArray &extras : {QByteArray(), deepOutline,
             QByteArray("<Outlines><OutlineElem Title=\"root\"/></Outlines>")}) {
        QTemporaryDir dir;
        ASSERT_TRUE(dir.isValid());
        const QString path = createOfdFixture(dir, {}, explicitPageArea, extras);
        ASSERT_FALSE(path.isEmpty());
        Document::Error error;
        std::unique_ptr<OfdDocument> doc(OfdDocument::loadDocument(path, error));
        ASSERT_NE(doc, nullptr);
        std::vector<std::future<Outline>> queries;
        for (int i = 0; i < 8; ++i)
            queries.push_back(std::async(std::launch::async, [&doc] { return doc->outline(); }));
        const int expected = extras.contains("Title=\"root\"") ? 1 : 0;
        for (auto &query : queries)
            EXPECT_EQ(query.get().size(), expected);
        EXPECT_EQ(doc->outline().size(), expected);
        std::unique_ptr<Page> good(doc->page(0));
        ASSERT_NE(good, nullptr);
        EXPECT_FALSE(good->render(210, 297).isNull());
    }
}

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

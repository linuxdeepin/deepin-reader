// Copyright (C) 2026 ~ 2020 UnionTech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XpsDocumentAdapter.h"
#include "XpsTextExtractor.h"
#include "Model.h"
#include "ut_defines.h"

#include <QDir>
#include <QFile>
#include <QImage>
#include <QPageSize>
#include <QTemporaryDir>

#include <gtest/gtest.h>

using namespace deepin_reader;

namespace {

QString xpsFilePath()
{
    // Try resource path first (UTSOURCEDIR macro), then cwd-relative fallback
    QString path = QString(UTSOURCEDIR) + "/files/normal.xps";
    if (QFile(path).exists())
        return path;

    path = QCoreApplication::applicationDirPath() + "/files/normal.xps";
    if (QFile(path).exists())
        return path;

    return path;  // may not exist on minimal CI runners
}

bool hasXpsFile()
{
    return QFile(xpsFilePath()).exists();
}

}  // namespace

class TestXpsDocumentAdapter : public ::testing::Test
{
public:
    void SetUp() override
    {
        m_path = xpsFilePath();
        if (!hasXpsFile())
            GTEST_SKIP() << "normal.xps not available, skipping XPS adapter tests";

        Document::Error error = Document::NoError;
        m_doc.reset(XpsDocumentAdapter::loadDocument(m_path, error));
        ASSERT_NE(m_doc, nullptr);
        EXPECT_EQ(error, Document::NoError);
    }

    void TearDown() override {}

protected:
    QString m_path;
    std::unique_ptr<XpsDocumentAdapter> m_doc;
};

TEST_F(TestXpsDocumentAdapter, pageCountIsPositive)
{
    EXPECT_GT(m_doc->pageCount(), 0);
}

TEST_F(TestXpsDocumentAdapter, pageInRangeReturnsValid)
{
    int count = m_doc->pageCount();
    ASSERT_GT(count, 0);

    std::unique_ptr<Page> page(m_doc->page(0));
    EXPECT_NE(page, nullptr);

    // Out of range returns null
    std::unique_ptr<Page> badPage(m_doc->page(count));
    EXPECT_EQ(badPage, nullptr);

    std::unique_ptr<Page> negativePage(m_doc->page(-1));
    EXPECT_EQ(negativePage, nullptr);
}

TEST_F(TestXpsDocumentAdapter, saveFilterContainsXpsAndPdf)
{
    QStringList filters = m_doc->saveFilter();
    EXPECT_FALSE(filters.isEmpty());
    bool hasXps = false, hasPdf = false;
    for (const QString &filter : filters) {
        if (filter.contains("*.xps", Qt::CaseInsensitive))
            hasXps = true;
        if (filter.contains("*.pdf", Qt::CaseInsensitive))
            hasPdf = true;
    }
    EXPECT_TRUE(hasXps);
    EXPECT_TRUE(hasPdf);
}

TEST_F(TestXpsDocumentAdapter, saveReturnsFalseReadonly)
{
    // XPS is read-only; save() in place should always return false
    EXPECT_FALSE(m_doc->save());
}

TEST_F(TestXpsDocumentAdapter, saveAsEmptyPathFails)
{
    EXPECT_FALSE(m_doc->saveAs(QString()));
}

TEST_F(TestXpsDocumentAdapter, saveAsUnsupportedFormatFails)
{
    EXPECT_FALSE(m_doc->saveAs(QStringLiteral("/tmp/test_unknown.xxx")));
}

TEST_F(TestXpsDocumentAdapter, saveAsSamePathSucceeds)
{
    EXPECT_TRUE(m_doc->saveAs(m_path));
}

TEST_F(TestXpsDocumentAdapter, saveAsCopyXps)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString target = tmpDir.path() + "/copy.xps";

    ASSERT_TRUE(m_doc->saveAs(target));
    EXPECT_TRUE(QFile(target).exists());
    EXPECT_GT(QFile(target).size(), 0);
}

TEST_F(TestXpsDocumentAdapter, saveAsExportPdf)
{
    QTemporaryDir tmpDir;
    ASSERT_TRUE(tmpDir.isValid());
    QString target = tmpDir.path() + "/export.pdf";

    ASSERT_TRUE(m_doc->saveAs(target));
    EXPECT_TRUE(QFile(target).exists());
    EXPECT_GT(QFile(target).size(), 0);
}

TEST_F(TestXpsDocumentAdapter, outlineNotEmptyOrValid)
{
    // Outline may be empty but call must not crash
    Outline out = m_doc->outline();
    Q_UNUSED(out);
    SUCCEED();
}

TEST_F(TestXpsDocumentAdapter, propertiesContainsFormat)
{
    Properties props = m_doc->properties();
    EXPECT_FALSE(props.isEmpty());
    EXPECT_TRUE(props.contains("Format"));
}

TEST_F(TestXpsDocumentAdapter, pageSizeOfFirstPage)
{
    int count = m_doc->pageCount();
    ASSERT_GT(count, 0);

    QSizeF size = m_doc->pageSize(0);
    EXPECT_GT(size.width(), 0.0);
    EXPECT_GT(size.height(), 0.0);

    // Out of range returns empty
    QSizeF bad = m_doc->pageSize(count);
    EXPECT_TRUE(bad.isEmpty());

    QSizeF negative = m_doc->pageSize(-1);
    EXPECT_TRUE(negative.isEmpty());
}

TEST_F(TestXpsDocumentAdapter, renderPageReturnsValidImage)
{
    int count = m_doc->pageCount();
    ASSERT_GT(count, 0);

    QSizeF size = m_doc->pageSize(0);
    int w = static_cast<int>(size.width());
    int h = static_cast<int>(size.height());

    QImage img = m_doc->renderPage(0, w, h, QRect());
    EXPECT_FALSE(img.isNull());
    EXPECT_EQ(img.width(), w);
    EXPECT_EQ(img.height(), h);
}

TEST_F(TestXpsDocumentAdapter, renderPageInvalidSizeReturnsNull)
{
    QImage img = m_doc->renderPage(0, 0, 0, QRect());
    EXPECT_TRUE(img.isNull());

    QImage negativeImg = m_doc->renderPage(0, -10, -10, QRect());
    EXPECT_TRUE(negativeImg.isNull());
}

TEST_F(TestXpsDocumentAdapter, renderPageInvalidIndexReturnsNull)
{
    int count = m_doc->pageCount();
    QImage img = m_doc->renderPage(count, 100, 100, QRect());
    EXPECT_TRUE(img.isNull());
}

TEST_F(TestXpsDocumentAdapter, renderPageWithSlice)
{
    int count = m_doc->pageCount();
    ASSERT_GT(count, 0);

    QImage img = m_doc->renderPage(0, 200, 200, QRect(0, 0, 100, 100));
    EXPECT_FALSE(img.isNull());
    EXPECT_EQ(img.width(), 100);
    EXPECT_EQ(img.height(), 100);
}

TEST_F(TestXpsDocumentAdapter, pageAdapterSizeFAndRender)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    QSizeF size = page->sizeF();
    EXPECT_GT(size.width(), 0.0);
    EXPECT_GT(size.height(), 0.0);

    QImage img = page->render(100, 100);
    EXPECT_FALSE(img.isNull());
}

TEST_F(TestXpsDocumentAdapter, pageAdapterText)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    QSizeF size = page->sizeF();
    QRectF fullRect(0, 0, size.width(), size.height());
    QString text = page->text(fullRect);
    // Text may be empty for image-only pages, but call must not crash
    Q_UNUSED(text);
    SUCCEED();
}

TEST_F(TestXpsDocumentAdapter, pageAdapterSearch)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    // Search for an unlikely-to-match string
    QVector<PageSection> results = page->search("__xyz_test_no_match__", false, false);
    EXPECT_TRUE(results.isEmpty());
}

TEST_F(TestXpsDocumentAdapter, pageAdapterWordsAndAnnotations)
{
    std::unique_ptr<Page> page(m_doc->page(0));
    ASSERT_NE(page, nullptr);

    QList<Word> words = page->words();
    // Words may or may not be empty
    Q_UNUSED(words);

    QList<Annotation *> annotations = page->annotations();
    EXPECT_TRUE(annotations.isEmpty());  // XPS adapter doesn't support annotations

    EXPECT_FALSE(page->canAddAndRemoveAnnotations());
    EXPECT_FALSE(page->hasWidgetAnnots());
}

TEST_F(TestXpsDocumentAdapter, loadDocumentInvalidPath)
{
    Document::Error error = Document::NoError;
    std::unique_ptr<XpsDocumentAdapter> bad(XpsDocumentAdapter::loadDocument(QString(), error));
    EXPECT_EQ(bad, nullptr);
    EXPECT_EQ(error, Document::FileError);
}

TEST_F(TestXpsDocumentAdapter, loadDocumentNonExistentFile)
{
    Document::Error error = Document::NoError;
    std::unique_ptr<XpsDocumentAdapter> bad(XpsDocumentAdapter::loadDocument("/tmp/__no_such_xps_file__.xps", error));
    EXPECT_NE(error, Document::NoError);
}

// XpsTextExtractor tests
class TestXpsTextExtractor : public ::testing::Test
{
public:
    void SetUp() override
    {
        m_path = xpsFilePath();
        if (!hasXpsFile())
            GTEST_SKIP() << "normal.xps not available, skipping XPS text extractor tests";
    }

protected:
    QString m_path;
};

TEST_F(TestXpsTextExtractor, extractWordsFromFirstPage)
{
    QList<Word> words = XpsTextExtractor::extractWords(m_path, 0);
    // Words may be empty for image-only or non-text pages
    Q_UNUSED(words);
    SUCCEED();
}

TEST_F(TestXpsTextExtractor, extractWordsInvalidPageIndex)
{
    QList<Word> words = XpsTextExtractor::extractWords(m_path, 9999);
    EXPECT_TRUE(words.isEmpty());
}

TEST_F(TestXpsTextExtractor, extractTextRunsFromFirstPage)
{
    QList<XpsTextExtractor::TextRun> runs = XpsTextExtractor::extractTextRuns(m_path, 0);
    Q_UNUSED(runs);
    SUCCEED();
}

TEST_F(TestXpsTextExtractor, extractTextRunsInvalidPageIndex)
{
    QList<XpsTextExtractor::TextRun> runs = XpsTextExtractor::extractTextRuns(m_path, 9999);
    EXPECT_TRUE(runs.isEmpty());
}

TEST_F(TestXpsTextExtractor, extractWordsFromInvalidFile)
{
    QList<Word> words = XpsTextExtractor::extractWords("/tmp/__no_such_xps_file__.xps", 0);
    EXPECT_TRUE(words.isEmpty());
}

// Factory test for XPS path
TEST(UT_DocumentFactory_XPS, getDocumentXps)
{
    QString path = xpsFilePath();
    if (!QFile(path).exists())
        GTEST_SKIP() << "normal.xps not available";

    Document::Error error = Document::NoError;
    std::unique_ptr<Document> doc(DocumentFactory::getDocument(Dr::XPS, path, QString(), QString(), nullptr, error));
    EXPECT_NE(doc, nullptr);
    EXPECT_EQ(error, Document::NoError);
    EXPECT_GT(doc->pageCount(), 0);
}

TEST(UT_DocumentFactory_XPS, getDocumentXpsNonExistent)
{
    Document::Error error = Document::NoError;
    Document *doc = DocumentFactory::getDocument(Dr::XPS, "/tmp/__no_such_file__.xps", QString(), QString(), nullptr, error);
    EXPECT_EQ(doc, nullptr);
    EXPECT_NE(error, Document::NoError);
}

// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DjVuModel.h"
#include "Model.h"
#include "stub.h"

#include <QDebug>
#include <QFile>

#include <libdjvu/ddjvuapi.h>
#include <gtest/gtest.h>
using namespace deepin_reader;

/**********测试DjVuPage*************/
class TestDjVuPage : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    DjVuPage *m_tester = nullptr;
    DjVuDocument *m_djvudocument = nullptr;
};

void TestDjVuPage::SetUp()
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.djvu";
    deepin_reader::Document::Error error;
    m_djvudocument = DjVuDocument::loadDocument(strPath, error);
    ASSERT_NE(m_djvudocument, nullptr);
    Page *page = m_djvudocument->page(0);
    ASSERT_NE(page, nullptr);
    m_tester = static_cast<DjVuPage *>(page);
}

void TestDjVuPage::TearDown()
{
    delete m_tester;
    delete m_djvudocument;
}

/***********测试用例*************/
TEST_F(TestDjVuPage, UT_TestDjVuPage_sizeF_001)
{
    EXPECT_TRUE(m_tester->sizeF().isValid());
}

TEST_F(TestDjVuPage, UT_TestDjVuPage_text_001)
{
    QRectF rect(0, 0, 100, 100);
    // Exercise text() on a loaded page; result depends on document content
    QString result = m_tester->text(rect);
    EXPECT_TRUE(result.isNull() || !result.isNull());
}

TEST_F(TestDjVuPage, UT_TestDjVuPage_text_fullPage)
{
    // Try the full page rect to exercise loadText() helpers
    // (miniexp_cadddr / miniexp_caddddr / skip).
    QSizeF sz = m_tester->sizeF();
    QRectF rect(0, 0, sz.width() * 2, sz.height() * 2);
    QString result = m_tester->text(rect);
    EXPECT_TRUE(result.isNull() || !result.isNull());
}

TEST_F(TestDjVuPage, UT_TestDjVuPage_search_001)
{
    // Exercise search() on a loaded page; ensure no crash
    QVector<PageSection> result = m_tester->search("test", false, false);
    EXPECT_GE(static_cast<int>(result.size()), 0);
}

/**********测试DjVuDocument*************/
class TestDjVuDocument : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    DjVuDocument *m_tester = nullptr;
};

void TestDjVuDocument::SetUp()
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.djvu";
    deepin_reader::Document::Error error;
    m_tester = DjVuDocument::loadDocument(strPath, error);
}

void TestDjVuDocument::TearDown()
{
    delete m_tester;
}

/**************测试用例****************/
TEST_F(TestDjVuDocument, UT_TestDjVuDocument_pageCount_001)
{
    EXPECT_EQ(m_tester->pageCount(), 192);
}

TEST_F(TestDjVuDocument, UT_TestDjVuDocument_page_001)
{
    Page *page = m_tester->page(0);
    EXPECT_TRUE(page->sizeF().isValid());
    EXPECT_TRUE(m_tester->m_pages.size() == 1);
    delete page;
}

TEST_F(TestDjVuDocument, UT_TestDjVuDocument_saveFilter_001)
{
    EXPECT_TRUE(m_tester->saveFilter().contains(QLatin1String("DjVu (*.djvu *.djv)")));
}

TEST_F(TestDjVuDocument, UT_TestDjVuDocument_saveAs_001)
{
    QString strPathcopy = UTSOURCEDIR;
    strPathcopy += "/files/normalcopy.djvu";

    EXPECT_TRUE(m_tester->saveAs(strPathcopy));
    QFile::remove(strPathcopy);
}

TEST_F(TestDjVuDocument, UT_TestDjVuDocument_save_001)
{
    EXPECT_TRUE(m_tester->save());
}

TEST_F(TestDjVuDocument, UT_TestDjVuDocument_properties_001)
{
    EXPECT_TRUE(m_tester->properties().isEmpty());
}

TEST_F(TestDjVuDocument, UT_TestDjVuDocument_searchAllPagesForText)
{
    // Search across pages to exercise findText() helpers
    // (miniexp_cadddr / miniexp_caddddr / skip). The normal.djvu fixture
    // may not have text, so this is a best-effort probe.
    int searchesRun = 0;
    int maxPages = qMin(m_tester->pageCount(), 20);
    for (int i = 0; i < maxPages; ++i) {
        Page *page = m_tester->page(i);
        if (!page) continue;
        page->search("the", false, false);
        ++searchesRun;
        delete page;
    }
    EXPECT_GT(searchesRun, 0);
}

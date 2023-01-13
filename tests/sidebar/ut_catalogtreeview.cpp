// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "CatalogTreeView.h"
#include "DocSheet.h"

#include "stub.h"
#include "addr_pri.h"
#include <gtest/gtest.h>
#include <QTest>
#include <QListView>

class TestCatalogTreeView : public ::testing::Test
{
public:
    TestCatalogTreeView() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new CatalogTreeView(sheet);
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete sheet;
    }

protected:
    DocSheet *sheet = nullptr;
    CatalogTreeView *m_tester = nullptr;
};

TEST_F(TestCatalogTreeView, initTest)
{

}

ACCESS_PRIVATE_FIELD(CatalogTreeView, bool, rightnotifypagechanged);
TEST_F(TestCatalogTreeView, testsetRightControl)
{
    m_tester->setRightControl(true);
    EXPECT_TRUE(access_private_field::CatalogTreeViewrightnotifypagechanged(*m_tester) == true);
}

ACCESS_PRIVATE_FIELD(CatalogTreeView, DocSheet *, m_sheet);
ACCESS_PRIVATE_FUN(CatalogTreeView, void(const deepin_reader::Section &, QStandardItem *), parseCatalogData);
TEST_F(TestCatalogTreeView, testparseCatalogData)
{
    call_private_fun::CatalogTreeViewparseCatalogData(*m_tester, deepin_reader::Section(), nullptr);
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_sheet(*m_tester)  != nullptr);
}

ACCESS_PRIVATE_FUN(CatalogTreeView, QList<QStandardItem *>(const QString &, const int &, const qreal &, const qreal &), getItemList);
TEST_F(TestCatalogTreeView, testgetItemList)
{
    QList<QStandardItem *> listItem = call_private_fun::CatalogTreeViewgetItemList(*m_tester, "1", 0, 1, 1);
    EXPECT_TRUE(listItem.count() == 2);
    qDeleteAll(listItem);
}

ACCESS_PRIVATE_FIELD(CatalogTreeView, int, m_index);
TEST_F(TestCatalogTreeView, testhandleOpenSuccess)
{
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_index(*m_tester) == 0);
}

ACCESS_PRIVATE_FIELD(CatalogTreeView, QString, m_title);
ACCESS_PRIVATE_FUN(CatalogTreeView, void(const QModelIndex &), slotCollapsed);
TEST_F(TestCatalogTreeView, testCollapsed)
{
    call_private_fun::CatalogTreeViewslotCollapsed(*m_tester, QModelIndex());
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_index(*m_tester) == -1);
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_title(*m_tester) == "");
}

ACCESS_PRIVATE_FUN(CatalogTreeView, void(const QModelIndex &), slotExpanded);
TEST_F(TestCatalogTreeView, testslotExpanded)
{
    call_private_fun::CatalogTreeViewslotExpanded(*m_tester, QModelIndex());
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_index(*m_tester) == -1);
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_title(*m_tester) == "");
}

ACCESS_PRIVATE_FUN(CatalogTreeView, void(const QModelIndex &, const QModelIndex &), currentChanged);
TEST_F(TestCatalogTreeView, testcurrentChanged)
{
    call_private_fun::CatalogTreeViewcurrentChanged(*m_tester, QModelIndex(), QModelIndex());
    EXPECT_TRUE(access_private_field::CatalogTreeViewrightnotifypagechanged(*m_tester) == false);
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_title(*m_tester) == "");
}

ACCESS_PRIVATE_FUN(CatalogTreeView, void(const QModelIndex &), onItemClicked);
TEST_F(TestCatalogTreeView, testonItemClicked)
{
    call_private_fun::CatalogTreeViewonItemClicked(*m_tester, QModelIndex());
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_title(*m_tester) == "");
}

static QString g_resizeCoulumnWidth_result;
void resizeCoulumnWidth_stub()
{
    g_resizeCoulumnWidth_result = __FUNCTION__;
}

ACCESS_PRIVATE_FUN(CatalogTreeView, void(), resizeCoulumnWidth);
ACCESS_PRIVATE_FUN(CatalogTreeView, void(QResizeEvent *), resizeEvent);
TEST_F(TestCatalogTreeView, testresizeEvent)
{
    auto TestCatalogTreeView_resizeCoulumnWidth = get_private_fun::CatalogTreeViewresizeCoulumnWidth();
    Stub stub;
    stub.set(TestCatalogTreeView_resizeCoulumnWidth, resizeCoulumnWidth_stub);
    QResizeEvent *e = new QResizeEvent(QSize(100, 100), QSize(80, 80));
    call_private_fun::CatalogTreeViewresizeEvent(*m_tester, e);
    delete e;
    EXPECT_TRUE(g_resizeCoulumnWidth_result == "resizeCoulumnWidth_stub");
}

ACCESS_PRIVATE_FUN(CatalogTreeView, void(QMouseEvent *), mousePressEvent);
TEST_F(TestCatalogTreeView, testmousePressEvent)
{
    QTest::mousePress(m_tester, Qt::LeftButton);
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    call_private_fun::CatalogTreeViewmousePressEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(access_private_field::CatalogTreeViewrightnotifypagechanged(*m_tester) == false);
}

TEST_F(TestCatalogTreeView, testkeyPressEvent)
{
    QTest::keyRelease(m_tester, Qt::Key_Enter);
    EXPECT_TRUE(access_private_field::CatalogTreeViewrightnotifypagechanged(*m_tester) == false);
}

TEST_F(TestCatalogTreeView, testsetIndex)
{
    m_tester->setIndex(0, "111");
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_index(*m_tester) == 0);
    EXPECT_TRUE(access_private_field::CatalogTreeViewm_title(*m_tester) == "111");
}

TEST_F(TestCatalogTreeView, testresizeCoulumnWidth)
{
    auto TestCatalogTreeView_resizeCoulumnWidth = get_private_fun::CatalogTreeViewresizeCoulumnWidth();
    Stub stub;
    stub.set(TestCatalogTreeView_resizeCoulumnWidth, resizeCoulumnWidth_stub);
    call_private_fun::CatalogTreeViewresizeCoulumnWidth(*m_tester);
    EXPECT_TRUE(g_resizeCoulumnWidth_result == "resizeCoulumnWidth_stub");
    stub.reset(TestCatalogTreeView_resizeCoulumnWidth);
}

static QString g_scrollToIndex_result;
void scrollToIndex_stub()
{
    g_scrollToIndex_result = __FUNCTION__;
}

ACCESS_PRIVATE_FUN(CatalogTreeView, void(const QModelIndex &), scrollToIndex);
TEST_F(TestCatalogTreeView, testnextPage)
{
    auto TestCatalogTreeView_scrollToIndex = get_private_fun::CatalogTreeViewscrollToIndex();
    Stub stub;
    stub.set(TestCatalogTreeView_scrollToIndex, scrollToIndex_stub);
    m_tester->nextPage();
    EXPECT_TRUE(g_scrollToIndex_result == "scrollToIndex_stub");
    stub.reset(TestCatalogTreeView_scrollToIndex);
}

TEST_F(TestCatalogTreeView, testpageDownPage)
{
    auto TestCatalogTreeView_scrollToIndex = get_private_fun::CatalogTreeViewscrollToIndex();
    Stub stub;
    stub.set(TestCatalogTreeView_scrollToIndex, scrollToIndex_stub);
    m_tester->pageDownPage();
    EXPECT_TRUE(g_scrollToIndex_result == "scrollToIndex_stub");
    stub.reset(TestCatalogTreeView_scrollToIndex);
}

TEST_F(TestCatalogTreeView, testprevPage)
{
    auto TestCatalogTreeView_scrollToIndex = get_private_fun::CatalogTreeViewscrollToIndex();
    Stub stub;
    stub.set(TestCatalogTreeView_scrollToIndex, scrollToIndex_stub);
    m_tester->prevPage();
    EXPECT_TRUE(g_scrollToIndex_result == "scrollToIndex_stub");
    stub.reset(TestCatalogTreeView_scrollToIndex);
}

TEST_F(TestCatalogTreeView, testpageUpPage)
{
    auto TestCatalogTreeView_scrollToIndex = get_private_fun::CatalogTreeViewscrollToIndex();
    Stub stub;
    stub.set(TestCatalogTreeView_scrollToIndex, scrollToIndex_stub);
    m_tester->pageUpPage();
    EXPECT_TRUE(g_scrollToIndex_result == "scrollToIndex_stub");
    stub.reset(TestCatalogTreeView_scrollToIndex);
}

TEST_F(TestCatalogTreeView, testscrollToIndex)
{
    access_private_field::CatalogTreeViewrightnotifypagechanged(*m_tester) = true;
    call_private_fun::CatalogTreeViewscrollToIndex(*m_tester, QModelIndex());
    EXPECT_TRUE(access_private_field::CatalogTreeViewrightnotifypagechanged(*m_tester) == true);
}

ACCESS_PRIVATE_FUN(CatalogTreeView, void(const QFont &), onFontChanged);
TEST_F(TestCatalogTreeView, testonFontChanged)
{
    auto TestCatalogTreeView_resizeCoulumnWidth = get_private_fun::CatalogTreeViewresizeCoulumnWidth();
    Stub stub;
    stub.set(TestCatalogTreeView_resizeCoulumnWidth, resizeCoulumnWidth_stub);
    call_private_fun::CatalogTreeViewonFontChanged(*m_tester, QFont());
    EXPECT_TRUE(g_resizeCoulumnWidth_result == "resizeCoulumnWidth_stub");
    stub.reset(TestCatalogTreeView_resizeCoulumnWidth);
}

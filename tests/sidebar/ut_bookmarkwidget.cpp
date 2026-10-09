// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BookMarkWidget.h"
#include "DocSheet.h"
#include "SideBarImageListview.h"
#include "MsgHeader.h"

#include "stub.h"

#include <DPushButton>
#include <dapplicationhelper.h>
#include <dpalette.h>
#include <DGuiApplicationHelper>

#include <gtest/gtest.h>
#include <QTest>
#include <QListView>

class TestBookMarkWidget : public ::testing::Test
{
public:
    TestBookMarkWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new BookMarkWidget(sheet);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete sheet;
        delete m_tester;
    }

protected:
    DocSheet *sheet = nullptr;
    BookMarkWidget *m_tester = nullptr;
};

TEST_F(TestBookMarkWidget, initTest)
{

}

TEST_F(TestBookMarkWidget, testprevPage)
{
    m_tester->prevPage();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(TestBookMarkWidget, testnextPage)
{
    m_tester->nextPage();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(TestBookMarkWidget, testpageUp)
{
    m_tester->pageUp();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(TestBookMarkWidget, testpageDown)
{
    m_tester->pageDown();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(TestBookMarkWidget, testhandleOpenSuccess)
{
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(m_tester->bIshandOpenSuccess == true);
}

TEST_F(TestBookMarkWidget, testhandlePage)
{
    m_tester->handlePage(0);
    EXPECT_TRUE(m_tester->m_pAddBookMarkBtn->isEnabled() == true);
}

TEST_F(TestBookMarkWidget, testhandleBookMark)
{
    m_tester->handleBookMark(0, 0);
    EXPECT_TRUE(m_tester->m_pImageListView != nullptr);
}

TEST_F(TestBookMarkWidget, testdeleteItemByKey)
{
    m_tester->deleteItemByKey();
    EXPECT_TRUE(m_tester->m_sheet->m_bookmarks.count() == 0);
}

TEST_F(TestBookMarkWidget, testdeleteAllItem)
{
    m_tester->deleteAllItem();
    EXPECT_TRUE(m_tester->m_sheet->m_bookmarks.count() == 0);
}

TEST_F(TestBookMarkWidget, testonAddBookMarkClicked)
{
    m_tester->onAddBookMarkClicked();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(TestBookMarkWidget, testadaptWindowSize)
{
    m_tester->adaptWindowSize(1);
    EXPECT_TRUE(m_tester->m_pImageListView->property("adaptScale") == 1);
    EXPECT_TRUE(m_tester->m_pImageListView->itemSize() == QSize(266, 80));
}

TEST_F(TestBookMarkWidget, testshowMenu)
{
    m_tester->showMenu();
    EXPECT_TRUE(m_tester->m_pImageListView != nullptr);
    EXPECT_TRUE(m_tester->m_pImageListView->count() == 0);
}

TEST_F(TestBookMarkWidget, testonUpdateTheme)
{
    m_tester->onUpdateTheme();
    Dtk::Gui::DPalette paFrame = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    EXPECT_TRUE(Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette() == paFrame);
}

static QString g_deleteItemByKey_result;
void deleteItemByKey_stub()
{
    g_deleteItemByKey_result = __FUNCTION__;
}


TEST_F(TestBookMarkWidget, testonListMenuClick)
{
    Stub stub;
    stub.set(ADDR(BookMarkWidget, deleteItemByKey), deleteItemByKey_stub);
    m_tester->onListMenuClick(E_MENU_ACTION::E_BOOKMARK_DELETE);
    EXPECT_TRUE(g_deleteItemByKey_result == "deleteItemByKey_stub");
}

// ==================== PMS 批次 2 补强 ====================
// BUG10332 (sev2): 添加书签后未保存/书签状态不同步。
// 回归意图: handleBookMark 添加/移除必须同步书签列表模型与按钮可用状态;
//          onAddBookMarkClicked 必须把当前页写入 DocSheet::m_bookmarks 持久化集合。

static int pageCount_bug10332_stub()
{
    return 64;
}

TEST_F(TestBookMarkWidget, BUG10332_handleBookMarkAddSyncsModelAndButton)
{
    int cur = m_tester->m_sheet->currentIndex();
    m_tester->handleBookMark(cur, 1);

    // 添加: 书签列表模型插入该页, 当前页添加按钮置灰
    EXPECT_TRUE(m_tester->m_pImageListView->count() == 1);
    EXPECT_FALSE(m_tester->m_pAddBookMarkBtn->isEnabled());

    m_tester->handleBookMark(cur, 0);

    // 移除: 模型同步移除, 按钮恢复可用
    EXPECT_TRUE(m_tester->m_pImageListView->count() == 0);
    EXPECT_TRUE(m_tester->m_pAddBookMarkBtn->isEnabled());
}

TEST_F(TestBookMarkWidget, BUG10332_onAddBookMarkClickedPersistsBookmark)
{
    // fixture 中 DocSheet 未打开文档, pageCount() 为 0 会触发 setBookMark 索引守卫;
    // 桩定页数使书签写入链路可验证。
    Stub s;
    s.set(ADDR(DocSheet, pageCount), pageCount_bug10332_stub);

    int page = m_tester->m_sheet->currentIndex();

    m_tester->onAddBookMarkClicked();

    // 书签进入 DocSheet 持久化集合（保存链路的数据源）
    EXPECT_TRUE(m_tester->m_sheet->m_bookmarks.contains(page));
}

TEST_F(TestBookMarkWidget, BUG10332_onAddBookMarkClickedNullSheetGuard)
{
    // m_sheet 置空后点击添加不应崩溃（QPointer 空守卫）
    m_tester->m_sheet = nullptr;
    m_tester->onAddBookMarkClicked();
    EXPECT_TRUE(m_tester->m_sheet.isNull());
    SUCCEED();
}

// BUG12219 (sev2): 书签列表聚焦时按 Del 删除书签崩溃（无效索引未守卫）。
// 回归意图: deleteItemByKey 对无效索引必须走守卫路径, 不触发 setBookMark。

static QString g_bug12219_setBookMark_called;
void setBookMark_bug12219_stub(int, int)
{
    g_bug12219_setBookMark_called = "called";
}

TEST_F(TestBookMarkWidget, BUG12219_deleteItemByKeyInvalidIndexGuard)
{
    // 空书签列表: 当前模型索引无效, 不应触发 setBookMark
    g_bug12219_setBookMark_called.clear();
    Stub s;
    s.set(ADDR(DocSheet, setBookMark), setBookMark_bug12219_stub);
    m_tester->deleteItemByKey();
    EXPECT_TRUE(g_bug12219_setBookMark_called.isEmpty());
}

TEST_F(TestBookMarkWidget, BUG12219_deleteItemByKeyValidIndexRemovesBookmark)
{
    int cur = m_tester->m_sheet->currentIndex();
    m_tester->m_sheet->setBookMark(cur, 1);
    EXPECT_TRUE(m_tester->m_sheet->m_bookmarks.contains(cur));

    g_bug12219_setBookMark_called.clear();
    Stub s;
    s.set(ADDR(DocSheet, setBookMark), setBookMark_bug12219_stub);

    // 通过 handleBookMark 先向列表模型插入书签项, 再选中后 Del 删除
    m_tester->handleBookMark(cur, 1);
    EXPECT_TRUE(m_tester->m_pImageListView->count() == 1);
    m_tester->m_pImageListView->selectionModel()->setCurrentIndex(
        m_tester->m_pImageListView->model()->index(0, 0), QItemSelectionModel::Select);
    m_tester->deleteItemByKey();
    EXPECT_TRUE(g_bug12219_setBookMark_called == "called");
}

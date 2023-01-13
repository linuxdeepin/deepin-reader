// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "SheetSidebar.h"
#include "DocSheet.h"
#include "ThumbnailWidget.h"
#include "CatalogWidget.h"
#include "BookMarkWidget.h"
#include "NotesWidget.h"
#include "SearchResWidget.h"
#include "SideBarImageListview.h"
#include "ut_common.h"

#include "stub.h"
#include "addr_pri.h"

//#include <QStackedLayout>
#include <QTest>
#include <gtest/gtest.h>

class UT_SheetSidebar : public ::testing::Test
{
public:
    UT_SheetSidebar() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new SheetSidebar(m_sheet, PREVIEW_THUMBNAIL | PREVIEW_CATALOG | PREVIEW_BOOKMARK | PREVIEW_NOTE);
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet = nullptr;
    SheetSidebar *m_tester = nullptr;
};

TEST_F(UT_SheetSidebar, initTest)
{

}

/**********访问私有函数及成员变量***********/
ACCESS_PRIVATE_FUN(SheetSidebar, void(int), onBtnClicked);
ACCESS_PRIVATE_FUN(SheetSidebar, void(), onHandleOpenSuccessDelay);
ACCESS_PRIVATE_FUN(SheetSidebar, void(), onHandWidgetDocOpenSuccess);
ACCESS_PRIVATE_FUN(SheetSidebar, DToolButton * (const QString &, const QString &), createBtn);
ACCESS_PRIVATE_FUN(SheetSidebar, void(QResizeEvent *), resizeEvent);
ACCESS_PRIVATE_FUN(SheetSidebar, void(const double &), adaptWindowSize);
ACCESS_PRIVATE_FUN(SheetSidebar, void(QShowEvent *), showEvent);
ACCESS_PRIVATE_FUN(SheetSidebar, void(), showMenu);
ACCESS_PRIVATE_FUN(SheetSidebar, void(), onJumpToPrevPage);
ACCESS_PRIVATE_FUN(SheetSidebar, void(const QString &), dealWithPressKey);
ACCESS_PRIVATE_FUN(SheetSidebar, void(), onJumpToPageUp);
ACCESS_PRIVATE_FUN(SheetSidebar, void(), onJumpToNextPage);
ACCESS_PRIVATE_FUN(SheetSidebar, void(), deleteItemByKey);
ACCESS_PRIVATE_FUN(SheetSidebar, void(), onJumpToPageDown);
ACCESS_PRIVATE_FUN(SheetSidebar, void(), onUpdateWidgetTheme);

ACCESS_PRIVATE_FIELD(SheetSidebar, QStackedLayout *, m_stackLayout);
ACCESS_PRIVATE_FIELD(SheetSidebar, BookMarkWidget *, m_bookmarkWidget);
ACCESS_PRIVATE_FIELD(SheetSidebar, CatalogWidget *, m_catalogWidget);
ACCESS_PRIVATE_FIELD(SheetSidebar, bool, m_bOpenDocOpenSuccess);
ACCESS_PRIVATE_FIELD(SheetSidebar, DocSheet *, m_sheet);
ACCESS_PRIVATE_FIELD(SheetSidebar, ThumbnailWidget *, m_thumbnailWidget);
ACCESS_PRIVATE_FIELD(BaseWidget, bool, bIshandOpenSuccess);
ACCESS_PRIVATE_FIELD(SheetSidebar, NotesWidget *, m_notesWidget);
ACCESS_PRIVATE_FIELD(SheetSidebar, SearchResWidget *, m_searchWidget);
ACCESS_PRIVATE_FIELD(SearchResWidget, QString, m_searchKey);
ACCESS_PRIVATE_FIELD(SheetSidebar, qreal, m_scale);

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onBtnClicked)
{
    call_private_fun::SheetSidebaronBtnClicked(*m_tester, 0);
    EXPECT_TRUE(access_private_field::SheetSidebarm_stackLayout(*m_tester) ->currentIndex() == 0);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_setBookMark)
{
    m_tester->setBookMark(0, 0);
    EXPECT_TRUE(access_private_field::SheetSidebarm_bookmarkWidget(*m_tester) != nullptr);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_setCurrentPage)
{
    m_tester->setCurrentPage(0);
    EXPECT_TRUE(access_private_field::SheetSidebarm_catalogWidget(*m_tester) != nullptr);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_handleOpenSuccess)
{
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(access_private_field::SheetSidebarm_bOpenDocOpenSuccess(*m_tester) == true);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onHandleOpenSuccessDelay)
{
    call_private_fun::SheetSidebaronHandleOpenSuccessDelay(*m_tester);
    EXPECT_TRUE(access_private_field::SheetSidebarm_sheet(*m_tester) != nullptr);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onHandWidgetDocOpenSuccess_001)
{
    Stub stub;
    UTCommon::stub_QWidget_isVisible(stub, true);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(0);
    call_private_fun::SheetSidebaronHandWidgetDocOpenSuccess(*m_tester);
    auto bIshandOpenSuccess = access_private_field::BaseWidgetbIshandOpenSuccess(*access_private_field::SheetSidebarm_thumbnailWidget(*m_tester));
    //EXPECT_TRUE(bIshandOpenSuccess == true);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onHandWidgetDocOpenSuccess_002)
{
    Stub stub;
    UTCommon::stub_QWidget_isVisible(stub, true);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(1);
    call_private_fun::SheetSidebaronHandWidgetDocOpenSuccess(*m_tester);
    auto bIshandOpenSuccess = access_private_field::BaseWidgetbIshandOpenSuccess(*access_private_field::SheetSidebarm_catalogWidget(*m_tester));
    //EXPECT_TRUE(bIshandOpenSuccess == true);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onHandWidgetDocOpenSuccess_003)
{
    Stub stub;
    UTCommon::stub_QWidget_isVisible(stub, true);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(2);
    call_private_fun::SheetSidebaronHandWidgetDocOpenSuccess(*m_tester);
    auto bIshandOpenSuccess = access_private_field::BaseWidgetbIshandOpenSuccess(*access_private_field::SheetSidebarm_bookmarkWidget(*m_tester));
    //EXPECT_TRUE(bIshandOpenSuccess == true);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onHandWidgetDocOpenSuccess_004)
{
    Stub stub;
    UTCommon::stub_QWidget_isVisible(stub, true);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(3);
    call_private_fun::SheetSidebaronHandWidgetDocOpenSuccess(*m_tester);
    auto bIshandOpenSuccess = access_private_field::BaseWidgetbIshandOpenSuccess(*access_private_field::SheetSidebarm_notesWidget(*m_tester));
    //EXPECT_TRUE(bIshandOpenSuccess == true);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_handleSearchStart)
{
    m_tester->handleSearchStart("123");
    auto m_searchKey = access_private_field::SearchResWidgetm_searchKey(*access_private_field::SheetSidebarm_searchWidget(*m_tester));
    EXPECT_TRUE(m_searchKey == "123");
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_handleSearchStop)
{
    m_tester->handleSearchStop();
    auto m_searchKey = access_private_field::SearchResWidgetm_searchKey(*access_private_field::SheetSidebarm_searchWidget(*m_tester));
    EXPECT_TRUE(m_searchKey == "");
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_handleFindFinished)
{
    EXPECT_TRUE(m_tester->handleFindFinished() == 0);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_handleRotate)
{
    m_tester->handleRotate();
    EXPECT_TRUE(access_private_field::SheetSidebarm_sheet(*m_tester) != nullptr);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_handleAnntationMsg)
{
    m_tester->handleAnntationMsg(0, 0, nullptr);
    EXPECT_TRUE(access_private_field::SheetSidebarm_sheet(*m_tester) != nullptr);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_createBtn)
{
    EXPECT_TRUE(call_private_fun::SheetSidebarcreateBtn(*m_tester, "123", "testBtn") != nullptr);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_resizeEvent)
{
    QResizeEvent *event = new QResizeEvent(QSize(100, 100), QSize(80, 80));
    call_private_fun::SheetSidebarresizeEvent(*m_tester, event);
    delete event;
    qInfo() << m_tester->width();
    EXPECT_TRUE(m_tester->width() == 266);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_adaptWindowSize)
{
    call_private_fun::SheetSidebaradaptWindowSize(*m_tester, 1.0);
    EXPECT_TRUE(access_private_field::SheetSidebarm_scale(*m_tester) == 1.0);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_keyPressEvent)
{
    QTest::keyPress(m_tester, Qt::Key_PageUp);
    EXPECT_TRUE(access_private_field::SheetSidebarm_sheet(*m_tester) != nullptr);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_showEvent)
{
    QShowEvent *event = new QShowEvent();
    call_private_fun::SheetSidebarshowEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(access_private_field::SheetSidebarm_sheet(*m_tester) != nullptr);
}

static QString g_funcname;
void SideBarImageListView_showMenu_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_showMenu)
{
    Stub stub;
    stub.set(ADDR(SideBarImageListView, showMenu), SideBarImageListView_showMenu_stub);
    call_private_fun::SheetSidebarshowMenu(*m_tester);
    EXPECT_TRUE(g_funcname != "SideBarImageListView_showMenu_stub");
    stub.reset(ADDR(SideBarImageListView, showMenu));
}

void SheetSidebar_onJumpToPrevPage_stub()
{
    g_funcname = __FUNCTION__;
}

void SheetSidebar_onJumpToPageUp_stub()
{
    g_funcname = __FUNCTION__;
}

void SheetSidebar_onJumpToNextPage_stub()
{
    g_funcname = __FUNCTION__;
}

void SheetSidebar_onJumpToPageDown_stub()
{
    g_funcname = __FUNCTION__;
}

void SheetSidebar_deleteItemByKey_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_dealWithPressKey_001)
{
    auto SheetSidebar_onJumpToPrevPage = get_private_fun::SheetSidebaronJumpToPrevPage();
    Stub stub;
    stub.set(SheetSidebar_onJumpToPrevPage, SheetSidebar_onJumpToPrevPage_stub);
    call_private_fun::SheetSidebardealWithPressKey(*m_tester, Dr::key_up);
    EXPECT_TRUE(g_funcname == "SheetSidebar_onJumpToPrevPage_stub");
    stub.reset(SheetSidebar_onJumpToPrevPage);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_dealWithPressKey_002)
{
    auto SheetSidebar_onJumpToPageUp = get_private_fun::SheetSidebaronJumpToPageUp();
    Stub stub;
    stub.set(SheetSidebar_onJumpToPageUp, SheetSidebar_onJumpToPageUp_stub);
    call_private_fun::SheetSidebardealWithPressKey(*m_tester, Dr::key_pgUp);
    EXPECT_TRUE(g_funcname == "SheetSidebar_onJumpToPageUp_stub");
    stub.reset(SheetSidebar_onJumpToPageUp);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_dealWithPressKey_003)
{
    auto SheetSidebar_onJumpToNextPage = get_private_fun::SheetSidebaronJumpToNextPage();
    Stub stub;
    stub.set(SheetSidebar_onJumpToNextPage, SheetSidebar_onJumpToNextPage_stub);
    call_private_fun::SheetSidebardealWithPressKey(*m_tester, Dr::key_down);
    EXPECT_TRUE(g_funcname == "SheetSidebar_onJumpToNextPage_stub");
    stub.reset(SheetSidebar_onJumpToNextPage);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_dealWithPressKey_004)
{
    auto SheetSidebar_onJumpToPageDown = get_private_fun::SheetSidebaronJumpToPageDown();
    Stub stub;
    stub.set(SheetSidebar_onJumpToPageDown, SheetSidebar_onJumpToPageDown_stub);
    call_private_fun::SheetSidebardealWithPressKey(*m_tester, Dr::key_pgDown);
    EXPECT_TRUE(g_funcname == "SheetSidebar_onJumpToPageDown_stub");
    stub.reset(SheetSidebar_onJumpToPageDown);
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_dealWithPressKey_005)
{
    auto SheetSidebar_deleteItemByKey = get_private_fun::SheetSidebardeleteItemByKey();
    Stub stub;
    stub.set(SheetSidebar_deleteItemByKey, SheetSidebar_deleteItemByKey_stub);
    call_private_fun::SheetSidebardealWithPressKey(*m_tester, Dr::key_delete);
    EXPECT_TRUE(g_funcname == "SheetSidebar_deleteItemByKey_stub");
    stub.reset(SheetSidebar_deleteItemByKey);
}

void ThumbnailWidget_prevPage_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPrevPage_001)
{
    Stub stub;
    stub.set(ADDR(ThumbnailWidget, prevPage), ThumbnailWidget_prevPage_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(0);
    call_private_fun::SheetSidebaronJumpToPrevPage(*m_tester);
    EXPECT_TRUE(g_funcname == "ThumbnailWidget_prevPage_stub");
}

void BookMarkWidget_prevPage_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPrevPage_002)
{
    Stub stub;
    stub.set(ADDR(BookMarkWidget, prevPage), BookMarkWidget_prevPage_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(2);
    call_private_fun::SheetSidebaronJumpToPrevPage(*m_tester);
    EXPECT_TRUE(g_funcname == "BookMarkWidget_prevPage_stub");
}

void NotesWidget_prevPage_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPrevPage_003)
{
    Stub stub;
    stub.set(ADDR(NotesWidget, prevPage), NotesWidget_prevPage_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(3);
    call_private_fun::SheetSidebaronJumpToPrevPage(*m_tester);
    EXPECT_TRUE(g_funcname == "NotesWidget_prevPage_stub");
}

void CatalogWidget_prevPage_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPrevPage_004)
{
    Stub stub;
    stub.set(ADDR(CatalogWidget, prevPage), CatalogWidget_prevPage_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(1);
    call_private_fun::SheetSidebaronJumpToPrevPage(*m_tester);
    EXPECT_TRUE(g_funcname == "CatalogWidget_prevPage_stub");
}

void ThumbnailWidget_pageUp_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPageUp_001)
{
    Stub stub;
    stub.set(ADDR(ThumbnailWidget, pageUp), ThumbnailWidget_pageUp_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(0);
    call_private_fun::SheetSidebaronJumpToPageUp(*m_tester);
    EXPECT_TRUE(g_funcname == "ThumbnailWidget_pageUp_stub");
}

void BookMarkWidget_pageUp_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPageUp_002)
{
    Stub stub;
    stub.set(ADDR(BookMarkWidget, pageUp), BookMarkWidget_pageUp_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(2);
    call_private_fun::SheetSidebaronJumpToPageUp(*m_tester);
    EXPECT_TRUE(g_funcname == "BookMarkWidget_pageUp_stub");
}

void NotesWidget_pageUp_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPageUp_003)
{
    Stub stub;
    stub.set(ADDR(NotesWidget, pageUp), NotesWidget_pageUp_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(3);
    call_private_fun::SheetSidebaronJumpToPageUp(*m_tester);
    EXPECT_TRUE(g_funcname == "NotesWidget_pageUp_stub");
}

void CatalogWidget_pageUp_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPageUp_004)
{
    Stub stub;
    stub.set(ADDR(CatalogWidget, pageUp), CatalogWidget_pageUp_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(1);
    call_private_fun::SheetSidebaronJumpToPageUp(*m_tester);
    EXPECT_TRUE(g_funcname == "CatalogWidget_pageUp_stub");
}

void ThumbnailWidget_nextPage_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToNextPage_001)
{
    Stub stub;
    stub.set(ADDR(ThumbnailWidget, nextPage), ThumbnailWidget_nextPage_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(0);
    call_private_fun::SheetSidebaronJumpToNextPage(*m_tester);
    EXPECT_TRUE(g_funcname == "ThumbnailWidget_nextPage_stub");
}

void BookMarkWidget_nextPage_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToNextPage_002)
{
    Stub stub;
    stub.set(ADDR(BookMarkWidget, nextPage), BookMarkWidget_nextPage_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(2);
    call_private_fun::SheetSidebaronJumpToNextPage(*m_tester);
    EXPECT_TRUE(g_funcname == "BookMarkWidget_nextPage_stub");
}

void NotesWidget_nextPage_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToNextPage_003)
{
    Stub stub;
    stub.set(ADDR(NotesWidget, nextPage), NotesWidget_nextPage_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(3);
    call_private_fun::SheetSidebaronJumpToNextPage(*m_tester);
    EXPECT_TRUE(g_funcname == "NotesWidget_nextPage_stub");
}

void CatalogWidget_nextPage_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToNextPage_004)
{
    Stub stub;
    stub.set(ADDR(CatalogWidget, nextPage), CatalogWidget_nextPage_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(1);
    call_private_fun::SheetSidebaronJumpToNextPage(*m_tester);
    EXPECT_TRUE(g_funcname == "CatalogWidget_nextPage_stub");
}

void ThumbnailWidget_pageDown_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPageDown_001)
{
    Stub stub;
    stub.set(ADDR(ThumbnailWidget, pageDown), ThumbnailWidget_pageDown_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(0);
    call_private_fun::SheetSidebaronJumpToPageDown(*m_tester);
    EXPECT_TRUE(g_funcname == "ThumbnailWidget_pageDown_stub");
}

void BookMarkWidget_pageDown_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPageDown_002)
{
    Stub stub;
    stub.set(ADDR(BookMarkWidget, pageDown), BookMarkWidget_pageDown_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(2);
    call_private_fun::SheetSidebaronJumpToPageDown(*m_tester);
    EXPECT_TRUE(g_funcname == "BookMarkWidget_pageDown_stub");
}

void NotesWidget_pageDown_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPageDown_003)
{
    Stub stub;
    stub.set(ADDR(NotesWidget, pageDown), NotesWidget_pageDown_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(3);
    call_private_fun::SheetSidebaronJumpToPageDown(*m_tester);
    EXPECT_TRUE(g_funcname == "NotesWidget_pageDown_stub");
}

void CatalogWidget_pageDown_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onJumpToPageDown_004)
{
    Stub stub;
    stub.set(ADDR(CatalogWidget, pageDown), CatalogWidget_pageDown_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(1);
    call_private_fun::SheetSidebaronJumpToPageDown(*m_tester);
    EXPECT_TRUE(g_funcname == "CatalogWidget_pageDown_stub");
}

void BookMarkWidget_deleteItemByKey_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_deleteItemByKey_001)
{
    Stub stub;
    stub.set(ADDR(BookMarkWidget, deleteItemByKey), BookMarkWidget_deleteItemByKey_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(2);
    call_private_fun::SheetSidebardeleteItemByKey(*m_tester);
    EXPECT_TRUE(g_funcname == "BookMarkWidget_deleteItemByKey_stub");
}

void NotesWidget_deleteItemByKey_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_deleteItemByKey_002)
{
    Stub stub;
    stub.set(ADDR(NotesWidget, deleteItemByKey), NotesWidget_deleteItemByKey_stub);
    access_private_field::SheetSidebarm_stackLayout(*m_tester)->setCurrentIndex(3);
    call_private_fun::SheetSidebardeleteItemByKey(*m_tester);
    EXPECT_TRUE(g_funcname == "NotesWidget_deleteItemByKey_stub");
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_onUpdateWidgetTheme)
{
    call_private_fun::SheetSidebaronUpdateWidgetTheme(*m_tester);
//    bool bTrue = false;
//    if (m_tester->m_btnGroupMap.count() > 0) {
//        bTrue = m_tester->m_btnGroupMap[0]->icon().isNull();
//    }
//    EXPECT_TRUE(bTrue == true);
}

void NotesWidget_changeResetModelData_stub()
{
    g_funcname = __FUNCTION__;
}

TEST_F(UT_SheetSidebar, UT_SheetSidebar_changeResetModelData)
{
    Stub stub;
    stub.set(ADDR(NotesWidget, changeResetModelData), NotesWidget_changeResetModelData_stub);
    m_tester->changeResetModelData();
    EXPECT_TRUE(g_funcname == "NotesWidget_changeResetModelData_stub");
}

// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "NotesWidget.h"
#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"
#include "DocSheet.h"
#include "MsgHeader.h"
#include "SaveDialog.h"
#include "SheetBrowser.h"
#include "ut_common.h"

#include "stub.h"
#include "addr_pri.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QListView>
#include <QPainter>

DWIDGET_USE_NAMESPACE

class UT_NotesWidget : public ::testing::Test
{
public:
    UT_NotesWidget() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new NotesWidget(m_sheet);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet = nullptr;
    NotesWidget *m_tester = nullptr;
};

TEST_F(UT_NotesWidget, initTest)
{

}

ACCESS_PRIVATE_FIELD(NotesWidget, QPointer<DocSheet>, m_sheet);
TEST_F(UT_NotesWidget, UT_NotesWidget_prevPage)
{
    m_tester->pageUp();
    EXPECT_TRUE(access_private_field::NotesWidgetm_sheet(*m_tester) != nullptr);
}

TEST_F(UT_NotesWidget, UT_NotesWidget_nextPage)
{
    m_tester->nextPage();
    EXPECT_TRUE(access_private_field::NotesWidgetm_sheet(*m_tester) != nullptr);
}

TEST_F(UT_NotesWidget, UT_NotesWidget_pageDown)
{
    m_tester->pageDown();
    EXPECT_TRUE(access_private_field::NotesWidgetm_sheet(*m_tester) != nullptr);
}

TEST_F(UT_NotesWidget, UT_NotesWidget_deleteItemByKey)
{
    m_tester->deleteItemByKey();
    EXPECT_TRUE(access_private_field::NotesWidgetm_sheet(*m_tester) != nullptr);
}

ACCESS_PRIVATE_FUN(NotesWidget, void(), deleteAllItem);
TEST_F(UT_NotesWidget, UT_NotesWidget_deleteAllItem)
{
    call_private_fun::NotesWidgetdeleteAllItem(*m_tester);
    EXPECT_TRUE(access_private_field::NotesWidgetm_sheet(*m_tester) != nullptr);
}

ACCESS_PRIVATE_FIELD(BaseWidget, bool, bIshandOpenSuccess);
TEST_F(UT_NotesWidget, UT_NotesWidget_handleOpenSuccess_001)
{
    access_private_field::BaseWidgetbIshandOpenSuccess(*m_tester) = true;
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(access_private_field::BaseWidgetbIshandOpenSuccess(*m_tester) == true);
}

TEST_F(UT_NotesWidget, UT_NotesWidget_handleOpenSuccess_002)
{
    access_private_field::BaseWidgetbIshandOpenSuccess(*m_tester) = false;
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(access_private_field::BaseWidgetbIshandOpenSuccess(*m_tester) == true);
}

ACCESS_PRIVATE_FUN(NotesWidget, void(const int &), onListMenuClick);
TEST_F(UT_NotesWidget, UT_NotesWidget_onListMenuClick_001)
{
    call_private_fun::NotesWidgetonListMenuClick(*m_tester, E_NOTE_COPY);
    EXPECT_TRUE(access_private_field::NotesWidgetm_sheet(*m_tester) != nullptr);
}

TEST_F(UT_NotesWidget, UT_NotesWidget_onListMenuClick_002)
{
    call_private_fun::NotesWidgetonListMenuClick(*m_tester, E_NOTE_DELETE);
    EXPECT_TRUE(access_private_field::NotesWidgetm_sheet(*m_tester) != nullptr);
}

static QString g_funcname;
int saveDialog_showTipDialog_stub()
{
    g_funcname = __FUNCTION__;
    return 1;
}

TEST_F(UT_NotesWidget, UT_NotesWidget_onListMenuClick_003)
{
    Stub stub;
    stub.set(ADDR(SaveDialog, showTipDialog), saveDialog_showTipDialog_stub);

    call_private_fun::NotesWidgetonListMenuClick(*m_tester, E_NOTE_DELETE_ALL);
    EXPECT_TRUE(g_funcname == "saveDialog_showTipDialog_stub");
    stub.reset(ADDR(SaveDialog, showTipDialog));
}

ACCESS_PRIVATE_FUN(NotesWidget, void(int), onListItemClicked);
TEST_F(UT_NotesWidget, UT_NotesWidget_onListItemClicked)
{
    call_private_fun::NotesWidgetonListItemClicked(*m_tester, 0);
    EXPECT_TRUE(access_private_field::NotesWidgetm_sheet(*m_tester) != nullptr);
}

ACCESS_PRIVATE_FUN(NotesWidget, void(), onAddAnnotation);
ACCESS_PRIVATE_FIELD(DocSheet, SheetBrowser *, m_browser);
ACCESS_PRIVATE_FIELD(SheetBrowser, bool, m_annotationInserting);
TEST_F(UT_NotesWidget, UT_NotesWidget_onAddAnnotation)
{
    call_private_fun::NotesWidgetonAddAnnotation(*m_tester);
    auto sheet = access_private_field::NotesWidgetm_sheet(*m_tester);
    auto browser = access_private_field::DocSheetm_browser(*sheet);
    auto annotationInserting = access_private_field::SheetBrowserm_annotationInserting(*browser);
    EXPECT_TRUE(annotationInserting == true);
}

ACCESS_PRIVATE_FUN(NotesWidget, void(), copyNoteContent);
TEST_F(UT_NotesWidget, UT_NotesWidget_copyNoteContent)
{
    call_private_fun::NotesWidgetcopyNoteContent(*m_tester);
    EXPECT_TRUE(access_private_field::NotesWidgetm_sheet(*m_tester) != nullptr);
}

ACCESS_PRIVATE_FIELD(NotesWidget, SideBarImageListView *, m_pImageListView);
TEST_F(UT_NotesWidget, UT_NotesWidget_adaptWindowSize)
{
    m_tester->adaptWindowSize(20);
    EXPECT_TRUE(access_private_field::NotesWidgetm_pImageListView(*m_tester)->property("adaptScale") == 20);

}

TEST_F(UT_NotesWidget, UT_NotesWidget_showMenu)
{
    Stub stub;
    UTCommon::stub_DMenu_exec(stub);
    m_tester->showMenu();
    EXPECT_TRUE(access_private_field::NotesWidgetm_pImageListView(*m_tester) != nullptr);
}

TEST_F(UT_NotesWidget, UT_NotesWidget_setTabOrderWidget)
{
    QList<QWidget *> listWgt;
    m_tester->setTabOrderWidget(listWgt);
    EXPECT_TRUE(listWgt.count() == 1);
}


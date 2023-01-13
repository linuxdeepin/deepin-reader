// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PagingWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"
#include "TMFunctionThread.h"
#include "stub.h"
#include "addr_pri.h"
#include <gtest/gtest.h>
#include <QTest>

namespace {
class TestPagingWidget : public ::testing::Test
{
public:
    TestPagingWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
        if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
            QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
            QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
        }

        sheet = new DocSheet(Dr::FileType::PDF, filePath, nullptr);
        m_tester = new PagingWidget(sheet);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete sheet;
        delete m_tester;
    }

protected:
    DocSheet *sheet = nullptr;
    PagingWidget *m_tester = nullptr;
};

static QString g_funcname;
void normalChangePage_stub()
{
    g_funcname = __FUNCTION__;
}

void setIndex_stub(int)
{
    g_funcname = __FUNCTION__;
}

void jumpToIndex_stub(int)
{
    g_funcname = __FUNCTION__;
}
void jumpToPage_stub(int)
{
    g_funcname = __FUNCTION__;
}
int pageCount_stub()
{
    return 10;
}
}

TEST_F(TestPagingWidget, initTest)
{

}
ACCESS_PRIVATE_FUN(PagingWidget, void(), slotUpdateTheme);
ACCESS_PRIVATE_FIELD(PagingWidget, DLabel *, m_pTotalPagesLab);
ACCESS_PRIVATE_FIELD(PagingWidget, DLabel *, m_pCurrentPageLab);

TEST_F(TestPagingWidget, testslotUpdateTheme)
{
    call_private_fun::PagingWidgetslotUpdateTheme(*m_tester);
    EXPECT_TRUE(access_private_field::PagingWidgetm_pTotalPagesLab(*m_tester)->foregroundRole() == DPalette::Text);
    EXPECT_TRUE(access_private_field::PagingWidgetm_pCurrentPageLab(*m_tester)->foregroundRole() == DPalette::Text);
}

TEST_F(TestPagingWidget, testresizeEvent)
{
    Stub s;
    s.set(ADDR(PagingWidget, setIndex), setIndex_stub);
    QResizeEvent *event = new QResizeEvent(QSize(100, 100), QSize(80, 80));
    m_tester->resizeEvent(event);
    delete event;
    EXPECT_TRUE(g_funcname == "setIndex_stub");
}

ACCESS_PRIVATE_FIELD(PagingWidget, int, m_curIndex);

TEST_F(TestPagingWidget, testsetIndex)
{
    access_private_field::PagingWidgetm_curIndex(*m_tester) = 1;
    m_tester->setIndex(0);
    EXPECT_EQ(access_private_field::PagingWidgetm_curIndex(*m_tester), 0);
}

ACCESS_PRIVATE_FIELD(PagingWidget, TMFunctionThread *, m_tmFuncThread);
TEST_F(TestPagingWidget, testhandleOpenSuccess)
{
    m_tester->handleOpenSuccess();
    access_private_field::PagingWidgetm_tmFuncThread(*m_tester)->wait();
    EXPECT_TRUE(access_private_field::PagingWidgetm_tmFuncThread(*m_tester)->result == access_private_field::PagingWidgetm_tmFuncThread(*m_tester)->func());
}

ACCESS_PRIVATE_FUN(PagingWidget, void(), normalChangePage);
ACCESS_PRIVATE_FUN(PagingWidget, void(), SlotJumpPageLineEditReturnPressed);
TEST_F(TestPagingWidget, testSlotJumpPageLineEditReturnPressed)
{
    Stub s;
    auto A_foo = get_private_fun::PagingWidgetnormalChangePage();
    s.set(A_foo, normalChangePage_stub);
    call_private_fun::PagingWidgetSlotJumpPageLineEditReturnPressed(*m_tester);
    EXPECT_TRUE(g_funcname == "normalChangePage_stub");
    s.reset(A_foo);
}

ACCESS_PRIVATE_FUN(PagingWidget, void(), onEditFinished);
TEST_F(TestPagingWidget, testonEditFinished)
{
    Stub s;
    s.set(ADDR(PagingWidget, setIndex), setIndex_stub);
    call_private_fun::PagingWidgetonEditFinished(*m_tester);
    EXPECT_TRUE(g_funcname == "setIndex_stub");
    s.reset(ADDR(PagingWidget, setIndex));
}

ACCESS_PRIVATE_FIELD(PagingWidget, DLineEdit *, m_pJumpPageLineEdit);
TEST_F(TestPagingWidget, testnormalChangePage)
{
    Stub s;
    s.set(ADDR(DocSheet, jumpToIndex), jumpToIndex_stub);
    s.set(ADDR(DocSheet, pageCount), pageCount_stub);

    access_private_field::PagingWidgetm_pJumpPageLineEdit(*m_tester)->setText("2");
    call_private_fun::PagingWidgetnormalChangePage(*m_tester);
    EXPECT_TRUE(g_funcname == "jumpToIndex_stub");
}

ACCESS_PRIVATE_FUN(PagingWidget, void(), pageNumberJump);
TEST_F(TestPagingWidget, testpageNumberJump)
{
    Stub s;
    auto A_foo = get_private_fun::PagingWidgetnormalChangePage();
    s.set(A_foo, normalChangePage_stub);
    call_private_fun::PagingWidgetpageNumberJump(*m_tester);
    EXPECT_TRUE(g_funcname == "normalChangePage_stub");
    s.reset(A_foo);
}

ACCESS_PRIVATE_FUN(PagingWidget, void(), slotPrePageBtnClicked);
TEST_F(TestPagingWidget, testslotPrePageBtnClicked)
{
    Stub s;
    s.set(ADDR(DocSheet, jumpToPage), jumpToPage_stub);
    call_private_fun::PagingWidgetslotPrePageBtnClicked(*m_tester);
    EXPECT_TRUE(g_funcname == "jumpToPage_stub");
}

ACCESS_PRIVATE_FUN(PagingWidget, void(), slotNextPageBtnClicked);
TEST_F(TestPagingWidget, testslotNextPageBtnClicked)
{
    Stub s;
    s.set(ADDR(DocSheet, jumpToPage), jumpToPage_stub);
    call_private_fun::PagingWidgetslotNextPageBtnClicked(*m_tester);
    EXPECT_TRUE(g_funcname == "jumpToPage_stub");
}

TEST_F(TestPagingWidget, testsetTabOrderWidget)
{
    QList<QWidget *> tabWidgetlst;
    m_tester->setTabOrderWidget(tabWidgetlst);
    EXPECT_EQ(tabWidgetlst.count(), 3);
}

ACCESS_PRIVATE_FIELD(PagingWidget, bool, m_bHasLabel);
ACCESS_PRIVATE_FUN(PagingWidget, void(), onFuncThreadFinished);

TEST_F(TestPagingWidget, testonFuncThreadFinished)
{
    access_private_field::PagingWidgetm_bHasLabel(*m_tester) = true;
    access_private_field::PagingWidgetm_curIndex(*m_tester) = 1;
    call_private_fun::PagingWidgetonFuncThreadFinished(*m_tester);
    EXPECT_FALSE(access_private_field::PagingWidgetm_bHasLabel(*m_tester));
    EXPECT_EQ(access_private_field::PagingWidgetm_curIndex(*m_tester), 0);
}

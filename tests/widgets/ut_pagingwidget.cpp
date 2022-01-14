/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PagingWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"
#include "TMFunctionThread.h"
#include "stub.h"

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

        m_tester = new PagingWidget(new DocSheet(Dr::FileType::PDF, filePath, m_tester));
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    PagingWidget *m_tester;
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

TEST_F(TestPagingWidget, testslotUpdateTheme)
{
    m_tester->slotUpdateTheme();
    EXPECT_TRUE(m_tester->m_pTotalPagesLab->foregroundRole() == DPalette::Text);
    EXPECT_TRUE(m_tester->m_pCurrentPageLab->foregroundRole() == DPalette::Text);
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

TEST_F(TestPagingWidget, testsetIndex)
{
    m_tester->m_curIndex = 1;
    m_tester->setIndex(0);
    EXPECT_EQ(m_tester->m_curIndex, 0);
}

TEST_F(TestPagingWidget, testhandleOpenSuccess)
{
    m_tester->handleOpenSuccess();

    m_tester->m_tmFuncThread->wait();
    EXPECT_TRUE(m_tester->m_tmFuncThread->result == m_tester->m_tmFuncThread->func());
}

TEST_F(TestPagingWidget, testSlotJumpPageLineEditReturnPressed)
{
    Stub s;
    s.set(ADDR(PagingWidget, normalChangePage), normalChangePage_stub);
    m_tester->SlotJumpPageLineEditReturnPressed();
    EXPECT_TRUE(g_funcname == "normalChangePage_stub");
}

TEST_F(TestPagingWidget, testonEditFinished)
{
    Stub s;
    s.set(ADDR(PagingWidget, setIndex), setIndex_stub);
    m_tester->onEditFinished();
    EXPECT_TRUE(g_funcname == "setIndex_stub");
}

TEST_F(TestPagingWidget, testnormalChangePage)
{
    Stub s;
    s.set(ADDR(DocSheet, jumpToIndex), jumpToIndex_stub);
    s.set(ADDR(DocSheet, pageCount), pageCount_stub);
    m_tester->m_pJumpPageLineEdit->setText("2");
    m_tester->normalChangePage();
    EXPECT_TRUE(g_funcname == "jumpToIndex_stub");
}

TEST_F(TestPagingWidget, testpageNumberJump)
{
    Stub s;
    s.set(ADDR(PagingWidget, normalChangePage), normalChangePage_stub);
    m_tester->pageNumberJump();
    EXPECT_TRUE(g_funcname == "normalChangePage_stub");
}

TEST_F(TestPagingWidget, testslotPrePageBtnClicked)
{
    Stub s;
    s.set(ADDR(DocSheet, jumpToPage), jumpToPage_stub);
    m_tester->slotPrePageBtnClicked();
    EXPECT_TRUE(g_funcname == "jumpToPage_stub");
}

TEST_F(TestPagingWidget, testslotNextPageBtnClicked)
{
    Stub s;
    s.set(ADDR(DocSheet, jumpToPage), jumpToPage_stub);
    m_tester->slotNextPageBtnClicked();
    EXPECT_TRUE(g_funcname == "jumpToPage_stub");
}

TEST_F(TestPagingWidget, testsetTabOrderWidget)
{
    QList<QWidget *> tabWidgetlst;
    m_tester->setTabOrderWidget(tabWidgetlst);
    EXPECT_EQ(tabWidgetlst.count(), 3);
}

TEST_F(TestPagingWidget, testonFuncThreadFinished)
{
    m_tester->m_bHasLabel = true;
    m_tester->m_curIndex = 1;
    m_tester->onFuncThreadFinished();
    EXPECT_FALSE(m_tester->m_bHasLabel);
    EXPECT_EQ(m_tester->m_curIndex, 0);
}

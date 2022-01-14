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

#include "FindWidget.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
namespace {
class TestFindWidget : public ::testing::Test
{
public:
    TestFindWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new FindWidget();
        QString filePath = QCoreApplication::applicationDirPath() + "/" + "files" + "/" + UT_FILE_PDF;
        if (!QFile(filePath).exists() && QFile(":/files/" + QString(UT_FILE_PDF)).exists()) {
            QDir().mkpath(QCoreApplication::applicationDirPath() + "/" + "files");
            QFile(":/files/" + QString(UT_FILE_PDF)).copy(filePath);
        }

        m_tester->setDocSheet(new DocSheet(Dr::FileType::PDF, filePath, m_tester));
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    FindWidget *m_tester;
};

static QString g_funcname;
void jumpToNextSearchResult_stub()
{
    g_funcname = __FUNCTION__;
}

void jumpToPrevSearchResult_stub()
{
    g_funcname = __FUNCTION__;
}

void setEditAlert_stub(const int &)
{
    g_funcname = __FUNCTION__;
}

void keyPressEvent_stub(void *, QKeyEvent *)
{
    g_funcname = __FUNCTION__;
}

void setFocus_stub()
{
    g_funcname = __FUNCTION__;
}

void DocSheet_startSearch_stub(const QString &)
{
    g_funcname = __FUNCTION__;
}
}

TEST_F(TestFindWidget, initTest)
{

}

TEST_F(TestFindWidget, testsetSearchEditFocus)
{
    Stub s;
    s.set((void(QWidget::*)())ADDR(QWidget, setFocus), setFocus_stub);
    m_tester->setSearchEditFocus();
    EXPECT_TRUE(g_funcname == "setFocus_stub");
}

TEST_F(TestFindWidget, testonSearchStop)
{
    m_tester->onSearchStop();
    EXPECT_TRUE(m_tester->m_lastSearchText.isEmpty());
    EXPECT_FALSE(m_tester->m_findPrevButton->isEnabled());
    EXPECT_FALSE(m_tester->m_findNextButton->isEnabled());

}

TEST_F(TestFindWidget, testonSearchStart)
{
    Stub stub;
    stub.set(ADDR(DocSheet, startSearch), DocSheet_startSearch_stub);
    m_tester->m_pSearchEdit->setText("123");
    m_tester->onSearchStart();
    EXPECT_TRUE(m_tester->m_lastSearchText == "123");
    EXPECT_TRUE(m_tester->m_findPrevButton->isEnabled());
    EXPECT_TRUE(m_tester->m_findNextButton->isEnabled());
}

TEST_F(TestFindWidget, testslotFindNextBtnClicked)
{
    Stub s;
    s.set(ADDR(DocSheet, jumpToNextSearchResult), jumpToNextSearchResult_stub);
    m_tester->slotFindNextBtnClicked();
    EXPECT_TRUE(g_funcname == "jumpToNextSearchResult_stub");
}

TEST_F(TestFindWidget, testslotFindPrevBtnClicked)
{
    Stub s;
    s.set(ADDR(DocSheet, jumpToPrevSearchResult), jumpToPrevSearchResult_stub);
    m_tester->slotFindPrevBtnClicked();
    EXPECT_TRUE(g_funcname == "jumpToPrevSearchResult_stub");
}

TEST_F(TestFindWidget, testonTextChanged)
{
    Stub s;
    s.set(ADDR(FindWidget, setEditAlert), jumpToPrevSearchResult_stub);
    m_tester->onTextChanged();
    EXPECT_TRUE(g_funcname == "jumpToPrevSearchResult_stub");
}

TEST_F(TestFindWidget, testsetEditAlert)
{
    m_tester->setEditAlert(1);
    EXPECT_FALSE(m_tester->m_pSearchEdit->isAlert());
}

TEST_F(TestFindWidget, testonCloseBtnClicked)
{
    m_tester->m_pSearchEdit->setText("123");
    m_tester->onCloseBtnClicked();
    EXPECT_TRUE(m_tester->m_pSearchEdit->text().isEmpty());
}

TEST_F(TestFindWidget, testkeyPressEvent)
{
    typedef void (*fptr)(DFloatingWidget *, QKeyEvent *);
    fptr DFloatingWidget_keyPressEvent = (fptr)(&DFloatingWidget::keyPressEvent);   //获取虚函数地址
    Stub s;
    s.set(DFloatingWidget_keyPressEvent, keyPressEvent_stub);
    g_funcname.clear();

    QTest::keyPress(m_tester, Qt::Key_Up);
    EXPECT_TRUE(g_funcname.isEmpty());

    QTest::keyPress(m_tester, Qt::Key_Delete);
    EXPECT_TRUE(g_funcname == "keyPressEvent_stub");
}

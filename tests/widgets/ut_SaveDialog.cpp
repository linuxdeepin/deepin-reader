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

#include "SaveDialog.h"

#include "stub.h"

#include <DDialog>

#include <gtest/gtest.h>
#include <QTest>

DWIDGET_USE_NAMESPACE

class TestSaveDialog : public ::testing::Test
{
public:
    TestSaveDialog(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new SaveDialog();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    SaveDialog *m_tester;
};
static QString g_funcname;
int dialog_exec_stub()
{
    g_funcname = __FUNCTION__;
    return 0;
}

TEST_F(TestSaveDialog, initTest)
{

}

TEST_F(TestSaveDialog, testshowExitDialog)
{
    Stub stub;
    typedef int (*fptr)(DDialog *);
    fptr A_foo = (fptr)(&DDialog::exec);   //获取虚函数地址
    stub.set(A_foo, dialog_exec_stub);

    EXPECT_EQ(m_tester->showExitDialog("1.pdf"), 0);
    EXPECT_TRUE(g_funcname == "dialog_exec_stub");
}

TEST_F(TestSaveDialog, testshowTipDialog)
{
    Stub stub;
    typedef int (*fptr)(DDialog *);
    fptr A_foo = (fptr)(&DDialog::exec);   //获取虚函数地址
    stub.set(A_foo, dialog_exec_stub);

    EXPECT_EQ(m_tester->showTipDialog("1.pdf"), 0);
    EXPECT_TRUE(g_funcname == "dialog_exec_stub");
}

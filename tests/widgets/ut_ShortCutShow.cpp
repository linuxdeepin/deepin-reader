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

#include "ShortCutShow.h"
#include "Global.h"
#include "DocSheet.h"
#include "ut_defines.h"
#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>

namespace {
class TestShortCutShow : public ::testing::Test
{
public:
    TestShortCutShow(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new ShortCutShow();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ShortCutShow *m_tester;
};

static QString g_funcname;
void initPDF_stub()
{
    g_funcname = __FUNCTION__;
}

void initDJVU_stub()
{
    g_funcname = __FUNCTION__;
}
}

TEST_F(TestShortCutShow, initTest)
{

}

TEST_F(TestShortCutShow, testsetSheet)
{
    Stub s;
    s.set(ADDR(ShortCutShow, initPDF), initPDF_stub);
    s.set(ADDR(ShortCutShow, initDJVU), initDJVU_stub);

    m_tester->setSheet(nullptr);
    EXPECT_TRUE(g_funcname == "initPDF_stub");

    DocSheet *sheet = new DocSheet(Dr::FileType::DJVU, "1.pdf");
    m_tester->setSheet(sheet);
    delete sheet;
    EXPECT_TRUE(g_funcname == "initDJVU_stub");

    sheet = new DocSheet(Dr::FileType::PDF, "1.pdf");
    m_tester->setSheet(sheet);
    delete sheet;
    EXPECT_TRUE(g_funcname == "initPDF_stub");
}

TEST_F(TestShortCutShow, testshow)
{

}


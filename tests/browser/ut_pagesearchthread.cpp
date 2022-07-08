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

#include "PageSearchThread.h"
#include "DocSheet.h"
#include "stub.h"

#include <gtest/gtest.h>

class UT_PageSearchThread : public ::testing::Test
{
public:
    UT_PageSearchThread(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new PageSearchThread();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    PageSearchThread *m_tester;
};

TEST_F(UT_PageSearchThread, initTest)
{

}

TEST_F(UT_PageSearchThread, UT_PageSearchThread_startSearch)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);

    m_tester->startSearch(sheet, "123");
    EXPECT_TRUE(m_tester->m_sheet == sheet);
    EXPECT_TRUE(m_tester->m_searchText == "123");
    m_tester->stopSearch();
    delete sheet;
}

TEST_F(UT_PageSearchThread, UT_PageSearchThread_stopSearch)
{
    m_tester->stopSearch();
    EXPECT_TRUE(m_tester->m_quit == true);
}

TEST_F(UT_PageSearchThread, UT_PageSearchThread_run)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);

    m_tester->startSearch(sheet, "123");
    m_tester->run();
    delete sheet;
}

TEST_F(UT_PageSearchThread, UT_PageSearchThread_initCJKtoKangxi)
{
    m_tester->initCJKtoKangxi();
}

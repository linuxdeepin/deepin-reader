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

#include "DocTabBar.h"
#include "DocSheet.h"
#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"

#include "stub.h"

#include <QTest>
#include <QListView>
#include <QPainter>
#include <QMimeData>

#include <gtest/gtest.h>

class UT_DocTabBar : public ::testing::Test
{
public:
    UT_DocTabBar(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new DocTabBar(nullptr);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    DocTabBar *m_tester;
};

TEST_F(UT_DocTabBar, initTest)
{

}

TEST_F(UT_DocTabBar, UT_DocTabBar_indexOfFilePath)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    EXPECT_TRUE(m_tester->indexOfFilePath(strPath) == -1);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_insertSheet_001)
{
    m_tester->insertSheet(nullptr);
    EXPECT_TRUE(m_tester->m_delayIndex == -1);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_insertSheet_002)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);
    EXPECT_TRUE(m_tester->m_delayIndex == 0);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_removeSheet)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);
    m_tester->removeSheet(sheet);
    EXPECT_TRUE(m_tester->count() == 0);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_showSheet)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);
    m_tester->showSheet(sheet);
    EXPECT_TRUE(m_tester->count() == 1);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_getSheets)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);
    EXPECT_TRUE(m_tester->getSheets().count() == 1);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_updateTabWidth)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);
    m_tester->updateTabWidth();
    EXPECT_TRUE(m_tester->tabRect(0).height() == 37);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_createMimeDataFromTab)
{
    QMimeData *p = m_tester->createMimeDataFromTab(0, QStyleOptionTab());
    EXPECT_TRUE(p->hasUrls() == false);
    delete p;
}

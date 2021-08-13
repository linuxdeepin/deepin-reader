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

#include "BookMarkWidget.h"
#include "DocSheet.h"

#include "stub.h"

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
        DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new BookMarkWidget(sheet);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    BookMarkWidget *m_tester;
};

TEST_F(TestBookMarkWidget, initTest)
{

}

TEST_F(TestBookMarkWidget, testprevPage)
{
    m_tester->prevPage();
}

TEST_F(TestBookMarkWidget, testpageUp)
{
    m_tester->pageUp();
}

TEST_F(TestBookMarkWidget, testnextPage)
{
    m_tester->pageUp();
}

TEST_F(TestBookMarkWidget, testpageDown)
{
    m_tester->pageUp();
}

TEST_F(TestBookMarkWidget, testhandleOpenSuccess)
{
    m_tester->handleOpenSuccess();
}

TEST_F(TestBookMarkWidget, testhandlePage)
{
    m_tester->handlePage(0);
}

TEST_F(TestBookMarkWidget, testhandleBookMark)
{
    m_tester->handleBookMark(0, 0);
}

TEST_F(TestBookMarkWidget, testdeleteItemByKey)
{
    m_tester->deleteItemByKey();
}

TEST_F(TestBookMarkWidget, testdeleteAllItem)
{
    m_tester->deleteAllItem();
}

TEST_F(TestBookMarkWidget, testonAddBookMarkClicked)
{
    m_tester->onAddBookMarkClicked();
}

TEST_F(TestBookMarkWidget, testadaptWindowSize)
{
    m_tester->adaptWindowSize(1);
}

TEST_F(TestBookMarkWidget, testshowMenu)
{
    m_tester->showMenu();
}

TEST_F(TestBookMarkWidget, testonUpdateTheme)
{
    m_tester->onUpdateTheme();
}

TEST_F(TestBookMarkWidget, testonListMenuClick)
{
    m_tester->onListMenuClick(0);
}

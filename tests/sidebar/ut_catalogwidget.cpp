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

#include "CatalogWidget.h"
#include "DocSheet.h"
#include "CatalogTreeView.h"

#include "stub.h"

#include <gtest/gtest.h>

class UT_CatalogWidget : public ::testing::Test
{
public:
    UT_CatalogWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new CatalogWidget(sheet);
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    CatalogWidget *m_tester;
};

TEST_F(UT_CatalogWidget, initTest)
{

}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_resizeEvent)
{
    m_tester->m_strTheme = "123";
    QResizeEvent *event = new QResizeEvent(QSize(100, 100), QSize(80, 80));
    m_tester->resizeEvent(event);
    delete event;
    EXPECT_TRUE(m_tester->titleLabel->text() == "123");
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_handleOpenSuccess)
{
    m_tester->m_strTheme = "123";
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(m_tester->titleLabel->text() == "");
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_handlePage)
{
    m_tester->handlePage(1);
    EXPECT_TRUE(m_tester->m_pTree->rightnotifypagechanged == true);
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_nextPage)
{
    m_tester->nextPage();
    EXPECT_TRUE(m_tester->m_pTree != nullptr);
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_prevPage)
{
    m_tester->prevPage();
    EXPECT_TRUE(m_tester->m_pTree != nullptr);
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_pageDown)
{
    m_tester->pageDown();
    EXPECT_TRUE(m_tester->m_pTree != nullptr);
}

TEST_F(UT_CatalogWidget, UT_CatalogWidget_pageUp)
{
    m_tester->pageUp();
    EXPECT_TRUE(m_tester->m_pTree != nullptr);
}

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

#include "ThumbnailWidget.h"
#include "DocSheet.h"
#include "CatalogTreeView.h"
#include "SideBarImageListview.h"
#include "PagingWidget.h"

#include "stub.h"

#include <QStackedLayout>
#include <gtest/gtest.h>

class UT_ThumbnailWidget : public ::testing::Test
{
public:
    UT_ThumbnailWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new ThumbnailWidget(sheet);
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ThumbnailWidget *m_tester;
};

TEST_F(UT_ThumbnailWidget, initTest)
{

}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_handleOpenSuccess_001)
{
    m_tester->bIshandOpenSuccess = true;
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(m_tester->bIshandOpenSuccess == true);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_handleOpenSuccess_002)
{
    m_tester->bIshandOpenSuccess = false;
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(m_tester->bIshandOpenSuccess == true);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_handleRotate)
{
    m_tester->handleRotate();
    EXPECT_TRUE(m_tester->m_pImageListView != nullptr);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_handlePage)
{
    m_tester->handlePage(0);
    EXPECT_TRUE(m_tester->m_pPageWidget->m_curIndex == 0);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_setBookMarkState)
{
    m_tester->setBookMarkState(0, 0);
    EXPECT_TRUE(m_tester->m_pImageListView->getImageModel() != nullptr);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_prevPage)
{
    m_tester->prevPage();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_pageUp)
{
    m_tester->prevPage();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_nextPage)
{
    m_tester->nextPage();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_pageDown)
{
    m_tester->pageDown();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_adaptWindowSize)
{
    m_tester->adaptWindowSize(1);
    EXPECT_TRUE(m_tester->m_pImageListView->property("adaptScale") == 1);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_scrollToCurrentPage)
{
    m_tester->scrollToCurrentPage();
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(UT_ThumbnailWidget, UT_ThumbnailWidget_setTabOrderWidget)
{
    QList<QWidget *> tabWidgetlst;
    m_tester->setTabOrderWidget(tabWidgetlst);
    EXPECT_EQ(tabWidgetlst.count(), 3);
}

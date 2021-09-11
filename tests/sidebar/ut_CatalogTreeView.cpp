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

#include "CatalogTreeView.h"
#include "DocSheet.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QListView>

class TestCatalogTreeView : public ::testing::Test
{
public:
    TestCatalogTreeView(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new CatalogTreeView(sheet);
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    CatalogTreeView *m_tester;
};

TEST_F(TestCatalogTreeView, initTest)
{

}

TEST_F(TestCatalogTreeView, testsetRightControl)
{
    m_tester->setRightControl(true);
    EXPECT_TRUE(m_tester->rightnotifypagechanged == true);
}

TEST_F(TestCatalogTreeView, testparseCatalogData)
{
    m_tester->parseCatalogData(deepin_reader::Section(), nullptr);
    EXPECT_TRUE(m_tester->m_sheet != nullptr);
}

TEST_F(TestCatalogTreeView, testgetItemList)
{
    QList<QStandardItem *> listItem = m_tester->getItemList("1", 0, 1, 1);
    EXPECT_TRUE(listItem.count() == 2);
    qDeleteAll(listItem);
}

TEST_F(TestCatalogTreeView, testhandleOpenSuccess)
{
    m_tester->handleOpenSuccess();
    EXPECT_TRUE(m_tester->m_index == 0);
}

TEST_F(TestCatalogTreeView, testCollapsed)
{
    m_tester->slotCollapsed(QModelIndex());
    EXPECT_TRUE(m_tester->m_index == -1);
    EXPECT_TRUE(m_tester->m_title == "");
}

TEST_F(TestCatalogTreeView, testslotExpanded)
{
    m_tester->slotExpanded(QModelIndex());
    EXPECT_TRUE(m_tester->m_index == -1);
    EXPECT_TRUE(m_tester->m_title == "");
}

TEST_F(TestCatalogTreeView, testcurrentChanged)
{
    m_tester->currentChanged(QModelIndex(), QModelIndex());
    EXPECT_TRUE(m_tester->rightnotifypagechanged == false);
    EXPECT_TRUE(m_tester->m_title == "");
}

TEST_F(TestCatalogTreeView, testonItemClicked)
{
    m_tester->onItemClicked(QModelIndex());
    EXPECT_TRUE(m_tester->m_title == "");
}

static QString g_resizeCoulumnWidth_result;
void resizeCoulumnWidth_stub()
{
    g_resizeCoulumnWidth_result = __FUNCTION__;
}

TEST_F(TestCatalogTreeView, testresizeEvent)
{
    Stub stub;
    stub.set(ADDR(CatalogTreeView, resizeCoulumnWidth), resizeCoulumnWidth_stub);
    QResizeEvent *e = new QResizeEvent(QSize(100, 100), QSize(80, 80));
    m_tester->resizeEvent(e);
    delete e;
    EXPECT_TRUE(g_resizeCoulumnWidth_result == "resizeCoulumnWidth_stub");
}

TEST_F(TestCatalogTreeView, testmousePressEvent)
{
    QTest::mousePress(m_tester, Qt::LeftButton);
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_tester->mousePressEvent(event);
    delete event;
    EXPECT_TRUE(m_tester->rightnotifypagechanged == false);
}

TEST_F(TestCatalogTreeView, testkeyPressEvent)
{
    QTest::keyRelease(m_tester, Qt::Key_Enter);
    EXPECT_TRUE(m_tester->rightnotifypagechanged == false);
}

TEST_F(TestCatalogTreeView, testsetIndex)
{
    m_tester->setIndex(0, "111");
    EXPECT_TRUE(m_tester->m_index == 0);
    EXPECT_TRUE(m_tester->m_title == "111");
}

TEST_F(TestCatalogTreeView, testresizeCoulumnWidth)
{
    Stub stub;
    stub.set(ADDR(CatalogTreeView, resizeCoulumnWidth), resizeCoulumnWidth_stub);
    m_tester->resizeCoulumnWidth();
    EXPECT_TRUE(g_resizeCoulumnWidth_result == "resizeCoulumnWidth_stub");
}

static QString g_scrollToIndex_result;
void scrollToIndex_stub()
{
    g_scrollToIndex_result = __FUNCTION__;
}

TEST_F(TestCatalogTreeView, testnextPage)
{
    Stub stub;
    stub.set(ADDR(CatalogTreeView, scrollToIndex), scrollToIndex_stub);
    m_tester->nextPage();
    EXPECT_TRUE(g_scrollToIndex_result == "scrollToIndex_stub");
}

TEST_F(TestCatalogTreeView, testpageDownPage)
{
    Stub stub;
    stub.set(ADDR(CatalogTreeView, scrollToIndex), scrollToIndex_stub);
    m_tester->pageDownPage();
    EXPECT_TRUE(g_scrollToIndex_result == "scrollToIndex_stub");
}

TEST_F(TestCatalogTreeView, testprevPage)
{
    Stub stub;
    stub.set(ADDR(CatalogTreeView, scrollToIndex), scrollToIndex_stub);
    m_tester->prevPage();
    EXPECT_TRUE(g_scrollToIndex_result == "scrollToIndex_stub");
}

TEST_F(TestCatalogTreeView, testpageUpPage)
{
    Stub stub;
    stub.set(ADDR(CatalogTreeView, scrollToIndex), scrollToIndex_stub);
    m_tester->pageUpPage();
    EXPECT_TRUE(g_scrollToIndex_result == "scrollToIndex_stub");
}

TEST_F(TestCatalogTreeView, testscrollToIndex)
{
    m_tester->rightnotifypagechanged = true;
    m_tester->scrollToIndex(QModelIndex());
    EXPECT_TRUE(m_tester->rightnotifypagechanged == true);
}

TEST_F(TestCatalogTreeView, testonFontChanged)
{
    Stub stub;
    stub.set(ADDR(CatalogTreeView, resizeCoulumnWidth), resizeCoulumnWidth_stub);
    m_tester->onFontChanged(QFont());
    EXPECT_TRUE(g_resizeCoulumnWidth_result == "resizeCoulumnWidth_stub");
}

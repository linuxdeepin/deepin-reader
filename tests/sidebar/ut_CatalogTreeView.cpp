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
}

TEST_F(TestCatalogTreeView, testparseCatalogData)
{
    m_tester->parseCatalogData(deepin_reader::Section(), nullptr);
}

TEST_F(TestCatalogTreeView, testgetItemList)
{
    QList<QStandardItem *> listItem = m_tester->getItemList("1", 0, 1, 1);
    qDeleteAll(listItem);
}

TEST_F(TestCatalogTreeView, testhandleOpenSuccess)
{
    m_tester->handleOpenSuccess();
}

TEST_F(TestCatalogTreeView, testCollapsed)
{
    m_tester->slotCollapsed(QModelIndex());
}

TEST_F(TestCatalogTreeView, testslotExpanded)
{
    m_tester->slotExpanded(QModelIndex());
}

TEST_F(TestCatalogTreeView, testcurrentChanged)
{
    m_tester->currentChanged(QModelIndex(), QModelIndex());
}

TEST_F(TestCatalogTreeView, testonItemClicked)
{
    m_tester->onItemClicked(QModelIndex());
}

TEST_F(TestCatalogTreeView, testresizeEventc)
{
    QResizeEvent *e = new QResizeEvent(QSize(100, 100), QSize(80, 80));
    m_tester->resizeEvent(e);
    delete e;
}

TEST_F(TestCatalogTreeView, testmousePressEvent)
{
    QTest::mousePress(m_tester, Qt::LeftButton);
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonPress, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    m_tester->mousePressEvent(event);
    delete event;
}

TEST_F(TestCatalogTreeView, testkeyPressEvent)
{
    QTest::keyRelease(m_tester, Qt::Key_Enter);
}

TEST_F(TestCatalogTreeView, testsetIndex)
{
    m_tester->setIndex(0, "111");
}

TEST_F(TestCatalogTreeView, testresizeCoulumnWidth)
{
    m_tester->resizeCoulumnWidth();
}

TEST_F(TestCatalogTreeView, testnextPage)
{
    m_tester->nextPage();
}

TEST_F(TestCatalogTreeView, testpageDownPage)
{
    m_tester->pageDownPage();
}

TEST_F(TestCatalogTreeView, testprevPage)
{
    m_tester->prevPage();
}

TEST_F(TestCatalogTreeView, testpageUpPage)
{
    m_tester->pageUpPage();
}

TEST_F(TestCatalogTreeView, testscrollToIndex)
{
    m_tester->scrollToIndex(QModelIndex());
}

TEST_F(TestCatalogTreeView, testonFontChanged)
{
    m_tester->onFontChanged(QFont());
}

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

#include "SideBarImageListview.h"
#include "DocSheet.h"
#include "SideBarImageViewModel.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QListView>

class TestSideBarImageListView : public ::testing::Test
{
public:
    TestSideBarImageListView(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        DocSheet *sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_tester = new SideBarImageListView(sheet);
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    SideBarImageListView *m_tester;
};

TEST_F(TestSideBarImageListView, initTest)
{

}

TEST_F(TestSideBarImageListView, testshowMenu)
{
    m_tester->showMenu();
}

TEST_F(TestSideBarImageListView, testsetListType)
{
    m_tester->setListType(0);
}

TEST_F(TestSideBarImageListView, testhandleOpenSuccess)
{
    m_tester->m_listType = E_SideBar::SIDE_THUMBNIL;
    m_tester->handleOpenSuccess();

    m_tester->m_listType = E_SideBar::SIDE_BOOKMARK;
    m_tester->handleOpenSuccess();

    m_tester->m_listType = E_SideBar::SIDE_NOTE;
    m_tester->handleOpenSuccess();
}

TEST_F(TestSideBarImageListView, testonItemClicked)
{
    m_tester->onItemClicked(QModelIndex());
}

TEST_F(TestSideBarImageListView, testonSetThumbnailListSlideGesture)
{
    m_tester->onSetThumbnailListSlideGesture();
}

TEST_F(TestSideBarImageListView, testonRemoveThumbnailListSlideGesture)
{
    m_tester->onRemoveThumbnailListSlideGesture();
}

TEST_F(TestSideBarImageListView, testscrollToIndex)
{
    m_tester->scrollToIndex(0, true);
}

TEST_F(TestSideBarImageListView, testscrollToModelInexPage)
{
    m_tester->scrollToModelInexPage(QModelIndex(), true);
}

TEST_F(TestSideBarImageListView, testmousePressEvent)
{
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonRelease, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);

    m_tester->m_listType = E_SideBar::SIDE_NOTE;
    m_tester->mousePressEvent(event);

    m_tester->m_listType = E_SideBar::SIDE_BOOKMARK;
    m_tester->mousePressEvent(event);

    delete event;
}

QAction *menu_exec_stub1(const QPoint &, QAction *)
{
    return nullptr;
}

TEST_F(TestSideBarImageListView, testshowNoteMenu)
{
    Stub stub;
    stub.set((QAction * (DMenu::*)(const QPoint &, QAction * at))ADDR(DMenu, exec), menu_exec_stub1);
    m_tester->showNoteMenu(QPoint(0, 0));
}

TEST_F(TestSideBarImageListView, testshowBookMarkMenu)
{
    Stub stub;
    stub.set((QAction * (DMenu::*)(const QPoint &, QAction * at))ADDR(DMenu, exec), menu_exec_stub1);
    m_tester->showBookMarkMenu(QPoint(0, 0));
}

TEST_F(TestSideBarImageListView, testgetModelIndexForPageIndex)
{
    m_tester->getModelIndexForPageIndex(0);
}

TEST_F(TestSideBarImageListView, testgetPageIndexForModelIndex)
{
    m_tester->getPageIndexForModelIndex(0);
}

TEST_F(TestSideBarImageListView, testpageUpIndex)
{
    m_tester->pageUpIndex();
}

TEST_F(TestSideBarImageListView, testpageDownIndex)
{
    m_tester->pageDownIndex();
}

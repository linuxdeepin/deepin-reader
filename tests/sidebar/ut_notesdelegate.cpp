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

#include "NotesDelegate.h"
#include "SideBarImageListview.h"
#include "SideBarImageViewModel.h"
#include "DocSheet.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QListView>
#include <QPainter>

class UT_NotesDelegate : public ::testing::Test
{
public:
    UT_NotesDelegate() {}

public:
    virtual void SetUp()
    {
        QString strPath = UTSOURCEDIR;
        strPath += "/files/1.pdf";
        m_sheet = new DocSheet(Dr::PDF, strPath, nullptr);
        m_pView = new SideBarImageListView(m_sheet);
        m_tester = new NotesDelegate(m_pView);
        m_pView->setItemDelegate(m_tester);
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
        delete m_pView;
        delete m_sheet;
    }

protected:
    DocSheet *m_sheet = nullptr;
    NotesDelegate *m_tester = nullptr;
    SideBarImageListView *m_pView = nullptr;
};

TEST_F(UT_NotesDelegate, initTest)
{

}

TEST_F(UT_NotesDelegate, UT_NotesDelegate_paint)
{
    m_pView->getImageModel()->insertPageIndex(1);
    QPainter *painter = new QPainter;
    QStyleOptionViewItem option;
    m_tester->paint(painter, option, m_pView->getImageModel()->index(0, 0));
    EXPECT_TRUE(m_tester->m_parent == m_pView);
    delete painter;
}

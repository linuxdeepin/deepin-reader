// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);

    QMimeData *p = m_tester->createMimeDataFromTab(0, QStyleOptionTab());
    EXPECT_TRUE(p->hasFormat("deepin_reader/tabbar"));
    EXPECT_TRUE(p->hasFormat("deepin_reader/uuid"));
    delete p;
    delete sheet;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_canInsertFromMimeData)
{
    QMimeData data;
    data.setData("deepin_reader/tabbar", "test");
    EXPECT_TRUE(m_tester->canInsertFromMimeData(0, &data));

    QMimeData emptyData;
    EXPECT_FALSE(m_tester->canInsertFromMimeData(0, &emptyData));
}

TEST_F(UT_DocTabBar, UT_DocTabBar_insertFromMimeDataOnDragEnter)
{
    QMimeData data;
    data.setData("deepin_reader/tabbar", "testTab");
    m_tester->insertFromMimeDataOnDragEnter(0, &data);
    EXPECT_EQ(m_tester->count(), 1);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_insertFromMimeDataOnDragEnterEmpty)
{
    QMimeData emptyData;
    m_tester->insertFromMimeDataOnDragEnter(0, &emptyData);
    EXPECT_EQ(m_tester->count(), 0);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_insertFromMimeDataEmpty)
{
    QMimeData emptyData;
    m_tester->insertFromMimeData(0, &emptyData);
    EXPECT_EQ(m_tester->count(), 0);
}

TEST_F(UT_DocTabBar, UT_DocTabBar_insertFromMimeDataValid)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);

    QMimeData data;
    data.setData("deepin_reader/uuid", DocSheet::getUuid(sheet).toString().toUtf8());
    m_tester->insertFromMimeData(0, &data);
    EXPECT_GE(m_tester->count(), 1);

    delete sheet;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onSetCurrentIndex)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);
    m_tester->m_delayIndex = 0;
    m_tester->onSetCurrentIndex();
    EXPECT_EQ(m_tester->currentIndex(), 0);
    delete sheet;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onTabChangedValid)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);
    m_tester->onTabChanged(0);
    SUCCEED();
    delete sheet;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onTabChangedInvalidIndex)
{
    m_tester->onTabChanged(-1);
    SUCCEED();
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onTabCloseRequestedInvalidSheet)
{
    m_tester->onTabCloseRequested(0);
    SUCCEED();
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onTabCloseRequestedValid)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);

    bool sigReceived = false;
    QObject::connect(m_tester, &DocTabBar::sigTabClosed, [&sigReceived](DocSheet *) {
        sigReceived = true;
    });
    m_tester->onTabCloseRequested(0);
    EXPECT_TRUE(sigReceived);
    delete sheet;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onTabCloseRequestedThrottle)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);

    int sigCount = 0;
    QObject::connect(m_tester, &DocTabBar::sigTabClosed, [&sigCount](DocSheet *) {
        sigCount++;
    });
    // First call should emit signal and start throttle timer
    m_tester->onTabCloseRequested(0);
    // Second call within 100ms should be throttled
    m_tester->onTabCloseRequested(0);
    EXPECT_EQ(sigCount, 1);
    delete sheet;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onTabReleasedSingleTab)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);
    // Single tab - should return early
    m_tester->onTabReleased(0);
    EXPECT_EQ(m_tester->count(), 1);
    delete sheet;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onTabDropedEmptyTarget)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet1 = new DocSheet(Dr::PDF, strPath, m_tester);
    DocSheet *sheet2 = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet1, 0);
    m_tester->insertSheet(sheet2, 1);

    // With null target and > 1 tabs, should remove current and emit sigTabNewWindow
    bool sigReceived = false;
    QObject::connect(m_tester, &DocTabBar::sigTabNewWindow, [&sigReceived](DocSheet *) {
        sigReceived = true;
    });
    m_tester->onTabDroped(0, Qt::CopyAction, nullptr);
    EXPECT_TRUE(sigReceived);
    EXPECT_EQ(m_tester->count(), 1);

    delete sheet1;
    delete sheet2;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onTabDropedSingleTabNullTarget)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet, 0);

    // Single tab with null target - should not allow
    m_tester->onTabDroped(0, Qt::CopyAction, nullptr);
    EXPECT_EQ(m_tester->count(), 1);
    delete sheet;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_onTabDropedMoveAction)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/1.pdf";
    DocSheet *sheet1 = new DocSheet(Dr::PDF, strPath, m_tester);
    DocSheet *sheet2 = new DocSheet(Dr::PDF, strPath, m_tester);
    m_tester->insertSheet(sheet1, 0);
    m_tester->insertSheet(sheet2, 1);

    QObject *target = new QObject;
    bool sigReceived = false;
    QObject::connect(m_tester, &DocTabBar::sigTabMoveOut, [&sigReceived](DocSheet *) {
        sigReceived = true;
    });
    m_tester->onTabDroped(0, Qt::MoveAction, target);
    EXPECT_TRUE(sigReceived);
    EXPECT_EQ(m_tester->count(), 1);

    delete target;
    delete sheet1;
    delete sheet2;
}

TEST_F(UT_DocTabBar, UT_DocTabBar_resizeEvent)
{
    QResizeEvent event(QSize(200, 50), QSize(100, 50));
    m_tester->resizeEvent(&event);
    SUCCEED();
}

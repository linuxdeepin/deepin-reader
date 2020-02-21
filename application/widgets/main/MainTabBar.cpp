/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#include "MainTabBar.h"

#include <QDebug>


MainTabBar::MainTabBar(DWidget *parent)
    : DTabBar(parent)
{
    this->setTabsClosable(true);
    this->setMovable(true);
//    this->setExpanding(true);
//    this->setDragable(true);
//    this->setStartDragDistance(40);

    __InitConnection();


    dApp->m_pModelService->addObserver(this);
}

MainTabBar::~MainTabBar()
{
    dApp->m_pModelService->removeObserver(this);
}

int MainTabBar::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk(msgContent);
    }
    return 0;
}

void MainTabBar::sendMsg(const int &, const QString &)
{

}

void MainTabBar::notifyMsg(const int &, const QString &)
{

}

void MainTabBar::resizeEvent(QResizeEvent *event)
{
    __SetTabMiniWidth();

    DTabBar::resizeEvent(event);
}

void MainTabBar::__InitConnection()
{
    connect(this, SIGNAL(tabBarClicked(int)), SLOT(SlotTabBarClicked(int)));
    connect(this, SIGNAL(tabCloseRequested(int)), SLOT(SlotTabCloseRequested(int)));
    connect(this, SIGNAL(tabAddRequested()), SLOT(SlotTabAddRequested()));
    connect(this, SIGNAL(tabDroped(int, Qt::DropAction, QObject *)), SLOT(SlotTabDroped(int, Qt::DropAction, QObject *)));

    connect(this, SIGNAL(sigOpenFileOk(const QString &)), SLOT(SlotOpenFileOk(const QString &)));
}

void MainTabBar::SlotTabBarClicked(int index)
{
    qDebug() << __FUNCTION__ << index;
}


void MainTabBar::__SetTabMiniWidth()
{
    int nCount = this->count();
    if (nCount == 0)
        return;

    int nWidth = this->width() / nCount;
    if (nWidth < 140) {
        nWidth = 140;
    }

    if (m_nOldMiniWidth != nWidth) {
        m_nOldMiniWidth = nWidth;

        for (int iLoop = 0; iLoop < nCount; iLoop++) {
            this->setTabMinimumSize(iLoop, QSize(nWidth, 36));
        }
    }
}

//  新增
void MainTabBar::SlotTabAddRequested()
{
    this->addTab("12312");

    __SetTabMiniWidth();

    qDebug() << __FUNCTION__;
}

//  关闭
void MainTabBar::SlotTabCloseRequested(int index)
{
    qDebug() << __FUNCTION__ << index;

    this->removeTab(index);

    __SetTabMiniWidth();
}

//void MainTabBar::SlotTabMoved(int nFrom, int nTo)
//{

//}

void MainTabBar::SlotTabDroped(int index, Qt::DropAction, QObject *target)
{
//    MainTabBar *tabbar = qobject_cast<MainTabBar *>(target);

//    if (tabbar == nullptr) {
//        MainWindow *window = static_cast<MainWindow *>(this->window());
//        window->move(QCursor::pos() - window->topLevelWidget()->pos());
//        window->show();
//        window->activateWindow();
//    } else {
//    qDebug() << __FUNCTION__ << "            1";
//        removeTab(index);
    //    }
}

void MainTabBar::SlotOpenFileOk(const QString &sContent)
{
    int iIndex = this->addTab(sContent);
    this->setTabData(iIndex, sContent);
}

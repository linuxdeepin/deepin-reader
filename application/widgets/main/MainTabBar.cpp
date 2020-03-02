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
#include <QDesktopServices>
#include <QUrl>

#include "FileDataModel.h"
#include "MsgModel.h"

#include "business/SaveDialog.h"
#include "business/FileDataManager.h"

#include "business/bridge/IHelper.h"

MainTabBar::MainTabBar(DWidget *parent)
    : DTabBar(parent)
{
    m_strObserverName = "MainTabBar";
    this->setTabsClosable(true);
    this->setMovable(true);

    m_pMsgList = {MSG_TAB_ADD, MSG_MENU_NEW_TAB,
                  MSG_MENU_OPEN_FOLDER,
                 };

    __InitConnection();

    dApp->m_pModelService->addObserver(this);
}

MainTabBar::~MainTabBar()
{
    dApp->m_pModelService->removeObserver(this);
}

int MainTabBar::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return  ConstantMsg::g_effective_res;
    }

    return 0;
}

void MainTabBar::sendMsg(const int &msgType, const QString &msgContent)
{
    notifyMsg(msgType, msgContent);
}

void MainTabBar::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
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
    connect(this, SIGNAL(currentChanged(int)), SLOT(SlotTabBarClicked(int)));

    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(SlotDealWithData(const int &, const QString &)));
}

void MainTabBar::SlotTabBarClicked(int index)
{
    QString sPath = this->tabData(index).toString();
    if (sPath != "") {
        emit sigTabBarIndexChange(sPath);
    }
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

void MainTabBar::AddFileTab(const QString &sContent)
{
    QStringList filePaths;

    QList<QString> sOpenFiles = dApp->m_pDataManager->qGetOpenFilePathList();

    QStringList canOpenFileList = sContent.split(Constant::sQStringSep, QString::SkipEmptyParts);
    foreach (auto s, canOpenFileList) {
        if (sOpenFiles.contains(s)) {
            notifyMsg(CENTRAL_SHOW_TIP, tr("The file is already open"));
        } else {
            filePaths.append(s);
        }
    }

    int nSize = filePaths.size();
    if (nSize > 0) {
        for (int iLoop = 0; iLoop < nSize; iLoop++) {
            QString s = filePaths.at(iLoop);
            if (s != "") {
                QString sName = getFileName(s);
                int iIndex = this->addTab(sName);
                this->setTabData(iIndex, s);

                emit sigAddTab(s);
            }
        }

        __SetTabMiniWidth();

        int nCurIndex = this->currentIndex();
        if (nCurIndex > -1) {
            SlotTabBarClicked(nCurIndex);
        }
    }
}

QString MainTabBar::getFileName(const QString &strFilePath)
{
    int nLastPos = strFilePath.lastIndexOf('/');
    nLastPos++;
    return strFilePath.mid(nLastPos);
}

//  新增
void MainTabBar::SlotTabAddRequested()
{
    notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_o);
}

//  关闭
void MainTabBar::SlotTabCloseRequested(int index)
{
    QString sPath = this->tabData(index).toString();
    if (sPath != "") {
        emit sigCloseTab(sPath);
    }
}

void MainTabBar::SlotRemoveFileTab(const QString &sPath)
{
    if (sPath != "") {
        int nCount = this->count();
        for (int iLoop = 0; iLoop < nCount; iLoop++) {
            QString sTabData = this->tabData(iLoop).toString();
            if (sTabData == sPath) {
                this->removeTab(iLoop);

                __SetTabMiniWidth();
                break;
            }
        }

        nCount = this->count();
        if (nCount == 0) {
            notifyMsg(CENTRAL_INDEX_CHANGE);
        }
    }
}

void MainTabBar::SlotDealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_TAB_ADD == msgType) {
        AddFileTab(msgContent);
    } else if (MSG_MENU_NEW_TAB == msgType) {
        SlotTabAddRequested();
    } else if (MSG_MENU_OPEN_FOLDER == msgType) {
        OpenCurFileFolder();
    }
}

void MainTabBar::OpenCurFileFolder()
{
    int nCurrentIndex = this->currentIndex();
    if (nCurrentIndex > -1) {
        QString sPath = this->tabData(nCurrentIndex).toString();
        if (sPath != "") {
            int nLastPos = sPath.lastIndexOf('/');
            sPath = sPath.mid(0, nLastPos);
            sPath = QString("file://") + sPath;
            QDesktopServices::openUrl(QUrl(sPath));
        }
    }
}

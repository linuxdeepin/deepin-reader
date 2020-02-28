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
#include "MainTabWidgetEx.h"

#include <QVBoxLayout>
#include <QStackedLayout>

#include "MainTabBar.h"
#include "MainSplitter.h"
#include "MsgModel.h"

#include "business/DocummentFileHelper.h"
#include "controller/FileDataManager.h"

#include "business/bridge/IHelper.h"

MainTabWidgetEx::MainTabWidgetEx(DWidget *parent)
    : CustomWidget("MainTabWidgetEx", parent)
{
    m_pMsgList = {MSG_EXIT_SAVE_FILE, MSG_EXIT_NOT_SAVE_FILE, MSG_EXIT_NOT_CHANGE_FILE,
                  MSG_DOC_OPEN_FILE_START
                 };

    initWidget();
    InitConnections();

    dApp->m_pModelService->addObserver(this);
}

MainTabWidgetEx::~MainTabWidgetEx()
{
    dApp->m_pModelService->removeObserver(this);
}

int MainTabWidgetEx::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_CLOSE_FILE || msgType == E_APP_EXIT) {
        emit sigCloseFile(msgType, msgContent);
    }

    return 0;
}

void MainTabWidgetEx::initWidget()
{
    MainTabBar *bar = new MainTabBar;
    connect(bar, SIGNAL(sigTabBarIndexChange(const QString &)), SLOT(SlotSetCurrentIndexFile(const QString &)));
    connect(bar, SIGNAL(sigAddTab(const QString &)), SLOT(SlotAddTab(const QString &)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(bar);

    m_pStackedLayout = new QStackedLayout;

    mainLayout->addItem(m_pStackedLayout);

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);
}

void MainTabWidgetEx::InitConnections()
{
    connect(this, SIGNAL(sigCloseFile(const int &, const QString &)), SLOT(SlotCloseFile(const int &, const QString &)));
    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(SlotDealWithData(const int &, const QString &)));
}

//  应用退出
void MainTabWidgetEx::AppExitFile(const int &msgType, const QString &sPath)
{
    QStringList pathList = sPath.split(Constant::sQStringSep, QString::SkipEmptyParts);
    foreach (const QString &text, pathList) {
        auto splitterList = this->findChildren<MainSplitter *>();
        foreach (auto s, splitterList) {
            QString sSplitterPath = s->qGetPath();
            if (sSplitterPath == text) {

                dApp->m_pHelper->qDealWithData(msgType, text);
                break;
            }
        }
    }
}


void MainTabWidgetEx::SlotDealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_EXIT_SAVE_FILE == msgType || MSG_EXIT_NOT_SAVE_FILE == msgType || MSG_EXIT_NOT_CHANGE_FILE == msgType) {
        AppExitFile(msgType, msgContent);
    } else if (MSG_DOC_OPEN_FILE_START == msgType) {
        SlotSetCurrentIndexFile(msgContent);
    }
}

void MainTabWidgetEx::SlotSetCurrentIndexFile(const QString &sPath)
{
    auto splitterList = this->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            int iIndex = m_pStackedLayout->indexOf(s);
            m_pStackedLayout->setCurrentIndex(iIndex);

            break;
        }
    }
}

void MainTabWidgetEx::SlotAddTab(const QString &sPath)
{
    if (m_pStackedLayout) {
        MainSplitter *splitter = new MainSplitter(this);
        splitter->qSetPath(sPath);
        m_pStackedLayout->addWidget(splitter);

        dApp->m_pHelper->qDealWithData(MSG_OPEN_FILE_PATH, sPath);
    }
}

void MainTabWidgetEx::SlotCloseFile(const int &, const QString &sPath)
{
    auto splitterList = this->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            m_pStackedLayout->removeWidget(s);

            delete  s;
            s = nullptr;

            break;
        }
    }
}

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
#include "controller/DataManager.h"

MainTabWidgetEx::MainTabWidgetEx(DWidget *parent)
    : CustomWidget("MainTabWidgetEx", parent)
{
    m_pMsgList = {MSG_TAB_ADD_OK, MSG_TAB_ADD_END,
                  MSG_EXIT_SAVE_FILE, MSG_EXIT_NOT_SAVE_FILE, MSG_EXIT_NOT_CHANGE_FILE,
                  MSG_TAB_SAVE_FILE, MSG_TAB_NOT_SAVE_FILE, MSG_TAB_NOT_CHANGE_SAVE_FILE,
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

    if (msgType == E_TAB_MSG) {
        emit sigTabMsg(msgContent);
    }

    return 0;
}

void MainTabWidgetEx::initWidget()
{
    MainTabBar *bar = new MainTabBar;

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
    connect(this, SIGNAL(sigTabMsg(const QString &)), SLOT(SlotTabMsg(const QString &)));

    connect(this, SIGNAL(sigCloseFile(const int &, const QString &)), SLOT(SlotCloseFile(const int &, const QString &)));
    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(SlotDealWithData(const int &, const QString &)));
}

void MainTabWidgetEx::AddFileLayout(const QString &msgContent)
{
    if (m_pStackedLayout) {
        MainSplitter *splitter = new MainSplitter(this);
        splitter->qSetPath(msgContent);
        m_pStackedLayout->addWidget(splitter);

        dApp->m_pDataManager->qInsertOpenFilePath(msgContent);
    }
}

void MainTabWidgetEx::AddFileLayoutEnd(const QString &msgContent)
{
    AddFileLayout(msgContent);

    notifyMsg(MSG_OPEN_FILE_PATH, msgContent);
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

                dApp->m_pDocummentFileHelper->qAppExitFile(msgType, text);
                break;
            }
        }
    }
}

//  tab 关闭删除文件
void MainTabWidgetEx::RemoveTabFile(const int &msgType, const QString &sPath)
{
    auto splitterList = this->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {

            dApp->m_pDocummentFileHelper->qRemoveTabFile(msgType, sPath);
            break;
        }
    }
}

void MainTabWidgetEx::SlotDealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_TAB_ADD_OK == msgType) {
        AddFileLayout(msgContent);
    } else if (MSG_TAB_ADD_END == msgType) {
        AddFileLayoutEnd(msgContent);
    } else if (MSG_EXIT_SAVE_FILE == msgType || MSG_EXIT_NOT_SAVE_FILE == msgType || MSG_EXIT_NOT_CHANGE_FILE == msgType) {
        AppExitFile(msgType, msgContent);
    } else if (MSG_TAB_SAVE_FILE == msgType || MSG_TAB_NOT_SAVE_FILE == msgType || MSG_TAB_NOT_CHANGE_SAVE_FILE == msgType) {
        RemoveTabFile(msgType, msgContent);
    } else if (MSG_DOC_OPEN_FILE_START == msgType) {
        setCurrentIndexFile(msgContent);
    }
}

void MainTabWidgetEx::SlotOpenFiles(const QString &filePaths)
{
    notifyMsg(MSG_TAB_ADD, filePaths);
}

void MainTabWidgetEx::SlotTabMsg(const QString &sContent)
{
    MsgModel mm;
    mm.fromJson(sContent);

    QString sPath = mm.getPath();

    int iMsg = mm.getMsgType();
    if (iMsg == MSG_TAB_SHOW_FILE_CHANGE) {
        setCurrentIndexFile(sPath);
    }
}

void MainTabWidgetEx::setCurrentIndexFile(const QString &sPath)
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

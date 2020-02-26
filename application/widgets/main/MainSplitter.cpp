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
#include "MainSplitter.h"

#include "../DocShowShellWidget.h"
#include "../LeftSidebarWidget.h"

#include "controller/FileDataManager.h"

#include "MsgModel.h"

MainSplitter::MainSplitter(CustomWidget *parent)
    : DSplitter(parent)
{
    dApp->m_pModelService->addObserver(this);

    InitWidget();
    InitConnections();
}

MainSplitter::~MainSplitter()
{
    dApp->m_pModelService->removeObserver(this);
}

void MainSplitter::InitWidget()
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    addWidget(new LeftSidebarWidget);
    addWidget(new DocShowShellWidget);

    QList<int> list_src;
    list_src.append(LEFTNORMALWIDTH);
    list_src.append(1000 - LEFTNORMALWIDTH);

    setSizes(list_src);
}

void MainSplitter::InitConnections()
{
    connect(this, SIGNAL(sigTitleMsgData(const QString &)), SLOT(SlotTitleMsg(const QString &)));
}

void MainSplitter::SlotTitleMsg(const QString &msgContent)
{
    auto children = this->findChildren<CustomWidget *>();
    foreach (auto sw, children) {
        sw->dealWithData(E_TITLE_MSG, msgContent);
    }
}

//  第一个 消息处理入口
int MainSplitter::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == E_TITLE_MSG) {       //  标题消息, 如果和当前路径显示路径 一样, 则截断消息
        QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();

        if (m_strPath == sCurPath) {
            emit sigTitleMsgData(msgContent);
            return ConstantMsg::g_effective_res;
        }
    }

    return 0;
}

void MainSplitter::sendMsg(const int &msgType, const QString &msgContent)
{
    notifyMsg(msgType, msgContent);
}

void MainSplitter::notifyMsg(const int &msgType, const QString &msgContent)
{
    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

QString MainSplitter::qGetPath() const
{
    return m_strPath;
}

void MainSplitter::qSetPath(const QString &strPath)
{
    m_strPath = strPath;

    auto docWidget = this->findChild<DocShowShellWidget *>();
    if (docWidget) {
        docWidget->qSetPath(strPath);
    }
}

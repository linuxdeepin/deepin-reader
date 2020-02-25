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
#include "../FileViewWidget.h"

MainSplitter::MainSplitter(CustomWidget *parent)
    : DSplitter(parent)
{
    InitWidget();
    InitConnections();

    dApp->m_pModelService->addObserver(this);
}

MainSplitter::~MainSplitter()
{
    dApp->m_pModelService->removeObserver(this);
}

void MainSplitter::InitWidget()
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_pLeftSidebarWidget = new LeftSidebarWidget;
    addWidget(m_pLeftSidebarWidget);

    m_pDocShellWidget = new DocShowShellWidget;
    addWidget(m_pDocShellWidget);

    QList<int> list_src;
    list_src.append(LEFTNORMALWIDTH);
    list_src.append(1000 - LEFTNORMALWIDTH);

    setSizes(list_src);
}

void MainSplitter::InitConnections()
{

}

int MainSplitter::dealWithData(const int &msgType, const QString &msgContent)
{
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

    m_pLeftSidebarWidget->qSetPath(m_strPath);
    m_pDocShellWidget->qSetPath(m_strPath);
}

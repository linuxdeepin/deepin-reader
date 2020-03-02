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

#include "MsgModel.h"

#include "../DocShowShellWidget.h"
#include "../LeftSidebarWidget.h"
#include "../FileViewWidget.h"
#include "../TitleWidget.h"

#include "business/FileDataManager.h"
#include "MainSplitterPrivate.h"
#include "menu/TitleMenu.h"

MainSplitter::MainSplitter(DWidget *parent)
    : DSplitter(parent)
{
    d_ptr = new MainSplitterPrivate(this);
    m_strObserverName = "MainSplitter";

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

    m_pLeftWidget = new LeftSidebarWidget;
    addWidget(m_pLeftWidget);

    m_pDocWidget = new DocShowShellWidget;
    connect(m_pDocWidget, SIGNAL(signalDealWithData(const int &, const QString &)), SLOT(SlotSplitterMsg(const int &, const QString &)));
    addWidget(m_pDocWidget);

    QList<int> list_src;
    list_src.append(LEFTNORMALWIDTH);
    list_src.append(1000 - LEFTNORMALWIDTH);

    setSizes(list_src);
}

void MainSplitter::InitConnections()
{
    connect(this, SIGNAL(sigDealWithDataMsg(const int &, const QString &)), SLOT(SlotDealWithDataMsg(const int &, const QString &)));
}

void MainSplitter::SlotDealWithDataMsg(const int &msgType, const QString &msgContent)
{
    QString s = msgContent;
    if (MSG_OPERATION_OPEN_FILE_OK == msgType) {
        s = d_ptr->qGetPath();
    }

    TitleWidget::Instance()->qDealWithData(msgType, msgContent);

    auto children = this->findChildren<CustomWidget *>();
    foreach (auto sw, children) {
        int nRes = sw->qDealWithData(msgType, s);
        if (nRes == MSG_OK) {
            break;
        }
    }
}

void MainSplitter::SlotSplitterMsg(const int &msgType, const QString &msgContent)
{
    SlotDealWithDataMsg(msgType, msgContent);
}

void MainSplitter::SlotNotifyMsg(const int &msgType, const QString &msgContent)
{
    if (this->isVisible()) {    //  只有显示的窗口 处理 MainTabWidgetEx 发送的信号
        QJsonParseError error;
        QJsonDocument doc = QJsonDocument::fromJson(msgContent.toLocal8Bit().data(), &error);
        if (error.error == QJsonParseError::NoError) {
            QJsonObject obj = doc.object();

            QString sContent = obj.value("content").toString();

            d_ptr->qDealWithData(msgType, sContent);

            int nRes = m_pLeftWidget->qDealWithData(msgType, sContent);
            if (nRes != MSG_OK) {
                m_pDocWidget->qDealWithData(msgType, sContent);
            }
        }
    }
}


void MainSplitter::SlotMainTabWidgetExMsg()
{
    if (this->isVisible()) {    //  只有显示的窗口 处理 MainTabWidgetEx 发送的信号
        qDebug() << this << "  222         " << __FUNCTION__  << this->qGetPath();
    }
}

//  第一个 消息处理入口
int MainSplitter::dealWithData(const int &msgType, const QString &msgContent)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(msgContent.toLocal8Bit().data(), &error);
    if (error.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        bool needPath = obj.value("need").toBool();
        if (needPath) {
            QString sPath = obj.value("path").toString();
            if (sPath == d_ptr->qGetPath()) {
                QString s = obj.value("content").toString();
                emit sigDealWithDataMsg(msgType, s);
                return ConstantMsg::g_effective_res;
            }
        }
    }

    return 0;
}

void MainSplitter::sendMsg(const int &msgType, const QString &msgContent)
{
//    notifyMsg(msgType, msgContent);
}

void MainSplitter::notifyMsg(const int &msgType, const QString &msgContent)
{
//    dApp->m_pModelService->notifyMsg(msgType, msgContent);
}

QString MainSplitter::qGetPath() const
{
    return d_ptr->qGetPath();
}

void MainSplitter::qSetPath(const QString &strPath)
{
    d_ptr->qSetPath(strPath);

    m_pDocWidget->OpenFilePath(strPath);  //  proxy 打开文件
}

void MainSplitter::qSetFileChange(const int &nState)
{
    d_ptr->qSetFileChange(nState);
}

int MainSplitter::qGetFileChange()
{
    return d_ptr->qGetFileChange();
}

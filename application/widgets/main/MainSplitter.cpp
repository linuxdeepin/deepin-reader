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

#include <QJsonDocument>
#include <QJsonObject>

//#include "../DocShowShellWidget.h"
#include "../LeftSidebarWidget.h"
#include "widgets/FileViewWidget.h"
#include "../TitleWidget.h"

#include "MainSplitterPrivate.h"
#include "menu/TitleMenu.h"

MainSplitter::MainSplitter(DWidget *parent)
    : DSplitter(parent), d_ptr(new MainSplitterPrivate(this))
{
    InitWidget();
}

MainSplitter::~MainSplitter()
{
}

void MainSplitter::InitWidget()
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    m_pLeftWidget = new LeftSidebarWidget;
    addWidget(m_pLeftWidget);

    m_pFileViewWidget = new FileViewWidget;
    connect(m_pFileViewWidget, SIGNAL(sigFileOpenOK(const QString &)), SLOT(SlotOpenFileOk(const QString &)));
    connect(m_pFileViewWidget, SIGNAL(sigFindOperation(const int &)), SLOT(SlotFindOperation(const int &)));
    connect(m_pFileViewWidget, SIGNAL(sigAnntationMsg(const int &, const QString &)), m_pLeftWidget, SIGNAL(sigAnntationMsg(const int &, const QString &)));

    addWidget(m_pFileViewWidget);

//    m_pDocWidget = new DocShowShellWidget;
//    connect(m_pDocWidget, SIGNAL(sigOpenFileOk()), SLOT(SlotOpenFileOk()));
//    connect(m_pDocWidget, SIGNAL(sigFindOperation(const int &)), SLOT(SlotFindOperation(const int &)));
//    addWidget(m_pDocWidget);

    QList<int> list_src;
    list_src.append(LEFTNORMALWIDTH);
    list_src.append(1000 - LEFTNORMALWIDTH);

    setSizes(list_src);
}

void MainSplitter::SlotOpenFileOk(const QString &s)
{
    TitleWidget::Instance()->dealWithData(MSG_OPERATION_OPEN_FILE_OK, s);

    m_pLeftWidget->dealWithData(MSG_OPERATION_OPEN_FILE_OK, s);
}

void MainSplitter::SlotFindOperation(const int &iType)
{
    m_pLeftWidget->SetFindOperation(iType);
}

void MainSplitter::SlotNotifyMsg(const int &msgType, const QString &msgContent)
{
    Q_D(MainSplitter);

    if (this->isVisible()) {    //  只有显示的窗口 处理 MainTabWidgetEx 发送的信号
        if (msgType == MSG_NOTIFY_KEY_MSG) {
            auto cwlist = this->findChildren<CustomWidget *>();
            foreach (auto cw, cwlist) {
                int nRes = cw->qDealWithShortKey(msgContent);
                if (nRes == MSG_OK) {
                    break;
                }
            }
        } else {
            QJsonParseError error;
            QJsonDocument doc = QJsonDocument::fromJson(msgContent.toLocal8Bit().data(), &error);
            if (error.error == QJsonParseError::NoError) {
                QJsonObject obj = doc.object();

                QString sContent = obj.value("content").toString();

                d->qDealWithData(msgType, sContent);

                int nRes = m_pLeftWidget->dealWithData(msgType, sContent);
                if (nRes != MSG_OK) {
                    nRes = m_pFileViewWidget->dealWithData(msgType, sContent);
                    if (nRes == MSG_OK)
                        return;
                }
            }
        }
    }
}

QString MainSplitter::qGetPath()
{
    Q_D(MainSplitter);
    return d->qGetPath();
}

void MainSplitter::qSetPath(const QString &strPath)
{
    Q_D(MainSplitter);
    d->qSetPath(strPath);

    m_pFileViewWidget->OpenFilePath(strPath);  //  proxy 打开文件
}

void MainSplitter::qSetFileChange(const int &nState)
{
    //d_ptr->qSetFileChange(nState);
    bool bchange = nState == 1 ? true : false;
    if (nullptr != m_pFileViewWidget)
        m_pFileViewWidget->setFileChange(bchange);
}

int MainSplitter::qGetFileChange()
{
    //return d_ptr->qGetFileChange();
    int istatus = -1;
    if (nullptr != m_pFileViewWidget)
        istatus = m_pFileViewWidget->getFileChange() ? 1 : 0;
    return  istatus;
}

void MainSplitter::saveData()
{
    Q_D(MainSplitter);
    d->saveData();
}

FileDataModel MainSplitter::qGetFileData()
{
    Q_D(MainSplitter);
    return d->qGetFileData();
}

void MainSplitter::setFileData(const FileDataModel &fdm)
{
    Q_D(MainSplitter);
    d->qSetFileData(fdm);
}

void MainSplitter::OnOpenSliderShow()
{
    m_bOldState = m_pLeftWidget->isVisible();
    m_pLeftWidget->setVisible(false);
}

void MainSplitter::OnExitSliderShow()
{
    m_pLeftWidget->setVisible(m_bOldState);
}

void MainSplitter::ShowFindWidget()
{
    m_pFileViewWidget->ShowFindWidget();
}

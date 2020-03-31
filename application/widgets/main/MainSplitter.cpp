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
#include <QStackedWidget>
#include <QMimeData>

#include "widgets/SpinnerWidget.h"
#include "widgets/LeftSidebarWidget.h"
#include "widgets/FileViewWidget.h"
#include "TitleWidget.h"
#include "widgets/main/MainTabWidgetEx.h"
#include "app/processcontroller.h"

MainSplitter::MainSplitter(DWidget *parent)
    : DSplitter(parent)
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
    if (m_pLeftWidget) {
        m_pLeftWidget->setFirstView(true);
    }
    addWidget(m_pLeftWidget);

    m_pFileViewWidget = new FileViewWidget;
    connect(m_pLeftWidget, SIGNAL(sigDeleteAnntation(const int &, const QString &)), m_pFileViewWidget, SIGNAL(sigDeleteAnntation(const int &, const QString &)));

    connect(m_pFileViewWidget, SIGNAL(sigFileOpenResult(const QString &, const bool &)), SLOT(SlotFileOpenResult(const QString &, const bool &)));
    connect(m_pFileViewWidget, SIGNAL(sigFindOperation(const int &)), SLOT(SlotFindOperation(const int &)));
    connect(m_pFileViewWidget, SIGNAL(sigAnntationMsg(const int &, const QString &)), m_pLeftWidget, SIGNAL(sigAnntationMsg(const int &, const QString &)));
    connect(m_pFileViewWidget, SIGNAL(sigBookMarkMsg(const int &, const QString &)), m_pLeftWidget, SIGNAL(sigBookMarkMsg(const int &, const QString &)));
    connect(m_pFileViewWidget, SIGNAL(sigUpdateThumbnail(const int &)), m_pLeftWidget, SIGNAL(sigUpdateThumbnail(const int &)));

    m_pRightWidget = new QStackedWidget;
    m_pSpinnerWidget = new SpinnerWidget(this);
    int tW = 36;
    int tH = 36;
    dApp->adaptScreenView(tW, tH);
    m_pSpinnerWidget->setSpinnerSize(QSize(tW, tW));
    m_pSpinnerWidget->startSpinner();

    m_pRightWidget->addWidget(m_pSpinnerWidget);
    m_pRightWidget->addWidget(m_pFileViewWidget);

    addWidget(m_pRightWidget);

    QList<int> list_src;
    tW = LEFTNORMALWIDTH;
    dApp->adaptScreenView(tW, tH);
    list_src.append(tW);
    tW = 1000 - LEFTNORMALWIDTH;
    dApp->adaptScreenView(tW, tH);
    list_src.append(tW);

    setSizes(list_src);

    if (m_pLeftWidget) {
        m_pLeftWidget->setFirstView(false);
    }

    setAcceptDrops(true);
}

void MainSplitter::SlotFileOpenResult(const QString &s, const bool &res)
{
    if (res) {
        if (m_pRightWidget && m_pSpinnerWidget) {
            m_pRightWidget->removeWidget(m_pSpinnerWidget);

            delete  m_pSpinnerWidget;
            m_pSpinnerWidget = nullptr;
        }

        if (this->isVisible()) {
            TitleWidget::Instance()->dealWithData(MSG_OPERATION_OPEN_FILE_OK, s);
        }

        m_pLeftWidget->dealWithData(MSG_OPERATION_OPEN_FILE_OK, s);
    }
    emit sigOpenFileResult(s, res);
}

void MainSplitter::SlotFindOperation(const int &iType)
{
    m_pLeftWidget->SetFindOperation(iType);
}

void MainSplitter::SlotNotifyMsg(const int &msgType, const QString &msgContent)
{
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
    return m_pFileViewWidget->getFilePath();
}

void MainSplitter::qSetPath(const QString &strPath)
{
    m_pFileViewWidget->OpenFilePath(strPath);  //  proxy 打开文件
}

void MainSplitter::qSetFileChange(const int &nState)
{
    bool bchange = nState == 1 ? true : false;
    if (nullptr != m_pFileViewWidget)
        m_pFileViewWidget->setFileChange(bchange);
}

int MainSplitter::qGetFileChange()
{
    int istatus = -1;
    if (nullptr != m_pFileViewWidget)
        istatus = m_pFileViewWidget->getFileChange() ? 1 : 0;
    return  istatus;
}

void MainSplitter::saveData()
{
    m_pFileViewWidget->saveData();
}

FileDataModel MainSplitter::qGetFileData()
{
    return m_pFileViewWidget->qGetFileData();
}

DocummentProxy *MainSplitter::getDocProxy()
{
    return m_pFileViewWidget->GetDocProxy();
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

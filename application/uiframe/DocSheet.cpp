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
#include "DocSheet.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QStackedWidget>
#include <QMimeData>

#include "widgets/SpinnerWidget.h"
#include "SheetSidebar.h"
#include "pdfControl/FileViewWidget.h"
#include "TitleWidget.h"
#include "CentralDocPage.h"
#include "app/ProcessController.h"

DocSheet::DocSheet(DWidget *parent)
    : DSplitter(parent)
{
    InitWidget();
}

DocSheet::~DocSheet()
{
}

void DocSheet::InitWidget()
{
    setHandleWidth(5);
    setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    if (dApp) {
        dApp->setFirstView(true);
    }

    m_sideBar = new SheetSidebar(this);
    addWidget(m_sideBar);

    m_pFileViewWidget = new FileViewWidget;
    connect(m_sideBar, SIGNAL(sigDeleteAnntation(const int &, const QString &)), m_pFileViewWidget, SIGNAL(sigDeleteAnntation(const int &, const QString &)));

    connect(m_pFileViewWidget, SIGNAL(sigFileOpenResult(const QString &, const bool &)), SLOT(SlotFileOpenResult(const QString &, const bool &)));
    connect(m_pFileViewWidget, SIGNAL(sigFindOperation(const int &)), SLOT(SlotFindOperation(const int &)));
    connect(m_pFileViewWidget, SIGNAL(sigAnntationMsg(const int &, const QString &)), m_sideBar, SIGNAL(sigAnntationMsg(const int &, const QString &)));
    connect(m_pFileViewWidget, SIGNAL(sigBookMarkMsg(const int &, const QString &)), m_sideBar, SIGNAL(sigBookMarkMsg(const int &, const QString &)));
    connect(m_pFileViewWidget, SIGNAL(sigUpdateThumbnail(const int &)), m_sideBar, SIGNAL(sigUpdateThumbnail(const int &)));

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

    setAcceptDrops(true);
}

void DocSheet::SlotFileOpenResult(const QString &s, const bool &res)
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

        m_sideBar->dealWithData(MSG_OPERATION_OPEN_FILE_OK, s);

        if (dApp) {
            dApp->setFirstView(false);
        }
    }
    emit sigOpenFileResult(s, res);
}

void DocSheet::SlotFindOperation(const int &iType)
{
    m_sideBar->SetFindOperation(iType);
}

void DocSheet::SlotNotifyMsg(const int &msgType, const QString &msgContent)
{
    if (this->isVisible()) {    //  只有显示的窗口 处理 CentralDocPage 发送的信号
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

                int nRes = m_sideBar->dealWithData(msgType, sContent);
                if (nRes != MSG_OK) {
                    nRes = m_pFileViewWidget->dealWithData(msgType, sContent);
                    if (nRes == MSG_OK)
                        return;
                }
            }
        }
    }
}

QString DocSheet::qGetPath()
{
    return m_pFileViewWidget->getFilePath();
}

void DocSheet::qSetPath(const QString &strPath)
{
    m_pFileViewWidget->OpenFilePath(strPath);  //  proxy 打开文件
}

void DocSheet::qSetFileChange(const int &nState)
{
    bool bchange = nState == 1 ? true : false;
    if (nullptr != m_pFileViewWidget)
        m_pFileViewWidget->setFileChange(bchange);
}

int DocSheet::qGetFileChange()
{
    int istatus = -1;
    if (nullptr != m_pFileViewWidget)
        istatus = m_pFileViewWidget->getFileChange() ? 1 : 0;
    return  istatus;
}

void DocSheet::saveData()
{
    m_pFileViewWidget->saveData();
}

FileDataModel DocSheet::qGetFileData()
{
    return m_pFileViewWidget->qGetFileData();
}

DocummentProxy *DocSheet::getDocProxy()
{
    return m_pFileViewWidget->GetDocProxy();
}

void DocSheet::OnOpenSliderShow()
{
    m_bOldState = m_sideBar->isVisible();
    m_sideBar->setVisible(false);
}

void DocSheet::OnExitSliderShow()
{
    m_sideBar->setVisible(m_bOldState);
}

void DocSheet::ShowFindWidget()
{
    m_pFileViewWidget->ShowFindWidget();
}

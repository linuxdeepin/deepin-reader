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

#include "business/SaveDialog.h"
#include "../FileAttrWidget.h"

#include "business/bridge/IHelper.h"

MainTabWidgetEx *MainTabWidgetEx::g_onlyApp = nullptr;

MainTabWidgetEx::MainTabWidgetEx(DWidget *parent)
    : CustomWidget("MainTabWidgetEx", parent)
{
    m_pMsgList = {MSG_OPERATION_ATTR, APP_EXIT};

    initWidget();
    InitConnections();
    g_onlyApp = this;

    dApp->m_pModelService->addObserver(this);
}

MainTabWidgetEx::~MainTabWidgetEx()
{
    dApp->m_pModelService->removeObserver(this);
}

MainTabWidgetEx *MainTabWidgetEx::Instance()
{
    return g_onlyApp;
}

int MainTabWidgetEx::dealWithData(const int &msgType, const QString &msgContent)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(msgContent.toLocal8Bit().data(), &error);
    if (error.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        QString sFrom = obj.value("from").toString();

        if (sFrom == "TitleWidget") {
            emit sigTitleMsg(msgType, msgContent);
            return ConstantMsg::g_effective_res;
        }
    } else {
        if (m_pMsgList.contains(msgType)) {
            emit sigDealWithData(msgType, msgContent);
            return ConstantMsg::g_effective_res;
        }
    }

    return 0;
}

QString MainTabWidgetEx::qGetCurPath() const
{
    QWidget *w = m_pStackedLayout->currentWidget();
    if (w) {
        auto pSplitter = qobject_cast<MainSplitter *>(w);
        if (pSplitter) {
            return pSplitter->qGetPath();
        }
    }

    return "";
}

int MainTabWidgetEx::GetCurFileState(const QString &sPath)
{
    auto splitterList = this->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            return  s->qGetFileChange();
        }
    }
    return -1;
}

void MainTabWidgetEx::InsertPathProxy(const QString &sPath, DocummentProxy *proxy)
{
    m_strOpenFileAndProxy.insert(sPath, proxy);
}

DocummentProxy *MainTabWidgetEx::getCurFileAndProxy(const QString &sPath) const
{
    if (m_strOpenFileAndProxy.contains(sPath)) {
        return m_strOpenFileAndProxy[sPath];
    }
    return  nullptr;
}

void MainTabWidgetEx::initWidget()
{
    m_pTabBar = new MainTabBar;
    connect(m_pTabBar, SIGNAL(sigTabBarIndexChange(const QString &)), SLOT(SlotSetCurrentIndexFile(const QString &)));
    connect(m_pTabBar, SIGNAL(sigAddTab(const QString &)), SLOT(SlotAddTab(const QString &)));
    connect(m_pTabBar, SIGNAL(sigCloseTab(const QString &)), SLOT(SlotCloseTab(const QString &)));

    connect(this, SIGNAL(sigRemoveFileTab(const QString &)), m_pTabBar, SLOT(SlotRemoveFileTab(const QString &)));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_pTabBar);

    m_pStackedLayout = new QStackedLayout;

    mainLayout->addItem(m_pStackedLayout);

    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);
    this->setLayout(mainLayout);
}

void MainTabWidgetEx::InitConnections()
{
    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), SLOT(SlotDealWithData(const int &, const QString &)));
}

void MainTabWidgetEx::onShowFileAttr()
{
    auto pFileAttrWidget = new FileAttrWidget;
    pFileAttrWidget->showScreenCenter();
}

//  应用退出, 删除所有文件
void MainTabWidgetEx::OnAppExit()
{
    QString saveFileList = "";
    QString noChangeFileList = "";

    auto splitterList = this->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        int iChange = s->qGetFileChange();
        if (iChange == 1) {
            saveFileList.append(sSplitterPath  + Constant::sQStringSep);
        } else {
            noChangeFileList.append(sSplitterPath + Constant::sQStringSep);
        }
    }

    if (saveFileList != "") {
        SaveDialog sd;
        int nRes = sd.showDialog();
        if (nRes <= 0) {
            return;
        }

        int nMsgType = MSG_NOT_SAVE_FILE;
        if (nRes == 2) {     //  保存
            nMsgType = MSG_SAVE_FILE;
        }
        dApp->m_pHelper->qDealWithData(nMsgType, saveFileList);
    }

    if (noChangeFileList != "") {
        dApp->m_pHelper->qDealWithData(MSG_NOT_CHANGE_SAVE_FILE, noChangeFileList);
    }

    dApp->exit(0);
}

void MainTabWidgetEx::SlotDealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_ATTR) {                            //  打开该文件的属性信息
        onShowFileAttr();
    } else if (msgType == APP_EXIT) {   //  关闭应用
        OnAppExit();
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
        connect(this, SIGNAL(sigTitleMsg(const int &, const QString &)), splitter, SLOT(SlotTitleMsg(const int &, const QString &)));
        connect(this, SIGNAL(sigDealWithData(const int &, const QString &)), splitter, SLOT(SlotDealWithDataMsg(const int &, const QString &)));
        splitter->qSetPath(sPath);
        m_pStackedLayout->addWidget(splitter);
    }
}

//  删除单个文件
void MainTabWidgetEx::SlotCloseTab(const QString &sPath)
{
    auto splitterList = this->findChildren<MainSplitter *>();
    foreach (auto s, splitterList) {
        QString sSplitterPath = s->qGetPath();
        if (sSplitterPath == sPath) {
            QString sRes  = "";
            int iChange = s->qGetFileChange();
            if (iChange == 1) {
                SaveDialog sd;
                int nRes = sd.showDialog();
                if (nRes <= 0) {
                    return;
                }

                int nMsgType = MSG_NOT_SAVE_FILE;
                if (nRes == 2) {     //  保存
                    nMsgType = MSG_SAVE_FILE;
                }

                QString s = sPath + Constant::sQStringSep;
                sRes = dApp->m_pHelper->qDealWithData(nMsgType, s);
            } else {
                QString s = sPath + Constant::sQStringSep;
                sRes = dApp->m_pHelper->qDealWithData(MSG_NOT_CHANGE_SAVE_FILE, s);
            }

            if (sRes != "") {
                QJsonParseError error;
                QJsonDocument doc = QJsonDocument::fromJson(sRes.toLocal8Bit().data(), &error);
                if (error.error == QJsonParseError::NoError) {
                    QJsonObject obj = doc.object();
                    int nReturn = obj.value("return").toInt();
                    if (nReturn == MSG_OK) {
                        m_strOpenFileAndProxy.remove(sPath);

                        emit sigRemoveFileTab(sPath);

                        m_pStackedLayout->removeWidget(s);

                        delete  s;
                        s = nullptr;
                    }
                }
            }
            break;
        }
    }
}

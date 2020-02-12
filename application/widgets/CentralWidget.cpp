/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
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

#include "CentralWidget.h"

#include <DSplitter>
#include <QFileInfo>
#include <QMimeData>
#include <QUrl>
#include <DSpinner>
#include <DDialog>
#include <DMessageManager>
#include <QStackedLayout>

#include "DocShowShellWidget.h"
#include "HomeWidget.h"
#include "LeftSidebarWidget.h"

#include "utils/utils.h"
#include "controller/DataManager.h"

CentralWidget::CentralWidget(CustomWidget *parent)
    : CustomWidget("CentralWidget", parent)
{
    setAcceptDrops(true);

    m_pMsgList = {MSG_OPERATION_OPEN_FILE_FAIL, MSG_OPERATION_OPEN_FILE_START, MSG_NOTIFY_SHOW_TIP};
    initWidget();
    initConnections();

    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

CentralWidget::~CentralWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

void CentralWidget::keyPressEvent(QKeyEvent *event)
{
    //  暂未想出好的处理方法  王志轩, 左侧栏 和 文档区域 上下键 功能不相同
    QStringList pFilterList = QStringList() << KeyStr::g_pgup << KeyStr::g_pgdown
                              << KeyStr::g_down << KeyStr::g_up
                              << KeyStr::g_left << KeyStr::g_right << KeyStr::g_del;
    QString key = Utils::getKeyshortcut(event);
    if (pFilterList.contains(key)) {
        QString sFilePath = DataManager::instance()->strOnlyFilePath();
        if (sFilePath != "") {
            notifyMsg(MSG_NOTIFY_KEY_MSG, key);
        }
    }

    CustomWidget::keyPressEvent(event);
}

void CentralWidget::initConnections()
{
    connect(this, SIGNAL(sigOpenFileOk()), SLOT(slotOpenFileOk()));

    connect(this, SIGNAL(sigDealWithData(const int &, const QString &)),
            SLOT(slotDealWithData(const int &, const QString &)));
}

//  文件打开成功
void CentralWidget::slotOpenFileOk()
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        pLayout->setCurrentIndex(1);

        //  打开成功, 删除转圈圈
        auto spinnerList = this->findChildren<DSpinner *>();
        foreach (auto sp, spinnerList) {
            if (sp->objectName() == "home_spinner") {

                delete  sp;
                sp = nullptr;

                break;
            }
        }
    }
}

void CentralWidget::slotDealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_FAIL) {
        onOpenFileFail(msgContent);
    } else if (msgType == MSG_OPERATION_OPEN_FILE_START) {
        onOpenFileStart();
    } else if (msgType == MSG_NOTIFY_SHOW_TIP) {
        onShowTips(msgContent);
    }
}

void CentralWidget::onOpenFileStart()
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {

        //  开始的时候  new 转圈圈
        auto pSpinnerWidget = new DWidget;
        QGridLayout *gridlyout = new QGridLayout(pSpinnerWidget);
        gridlyout->setAlignment(Qt::AlignCenter);
        auto m_spinner = new DSpinner(this);
        m_spinner->setObjectName("home_spinner");
        m_spinner->setFixedSize(60, 60);
        gridlyout->addWidget(m_spinner);
        m_spinner->start();

        pLayout->addWidget(pSpinnerWidget);

        pLayout->setCurrentIndex(2);
    }
}

//  文件打开失败
void CentralWidget::onOpenFileFail(const QString &errorInfo)
{
    //  打开失败, 停止转圈圈, 并且删除
    auto spinnerList = this->findChildren<DSpinner *>();
    foreach (auto sp, spinnerList) {
        if (sp->objectName() == "home_spinner") {
            sp->stop();

            delete sp;
            sp = nullptr;

            break;
        }
    }

    //  文档打开失败, 切换回首页
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        if (pLayout->currentIndex() != 0) {
            pLayout->setCurrentIndex(0);
        }
    }
    DDialog dlg("", errorInfo);
    dlg.setIcon(QIcon::fromTheme(ConstantMsg::g_app_name));
    dlg.addButtons(QStringList() << tr("OK"));
    dlg.exec();
}


void CentralWidget::onShowTips(const QString &contant)
{
    int position = contant.indexOf("##**");
    if (!contant.isEmpty() && position > 0) {
        QString strright = contant.mid(position + QString("##**").length());
        QString contents = contant.left(position);
        QString iconname(":/icons/deepin/builtin/%1.svg");
        if (strright == "warning ") {
            iconname = iconname.arg(strright);
        } else {
            iconname = iconname.arg(strright);
        }
        DMessageManager::instance()->sendMessage(this, QIcon(iconname), contents);

    } else {
        DMessageManager::instance()->sendMessage(this, QIcon(":/icons/deepin/builtin/ok.svg"), contant);
    }
}

int CentralWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (m_pMsgList.contains(msgType)) {
        emit sigDealWithData(msgType, msgContent);
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk();
    }

    return 0;
}

void CentralWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drag event if mime type is url.
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->accept();
    }
}

void CentralWidget::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        QStringList noOpenFileList;
        QStringList canOpenFileList;

        for (auto url : mimeData->urls()) {
            QString sFilePath = url.toLocalFile();

            QFileInfo file(sFilePath);
            if (file.isFile()) {
                QString sSuffix = file.completeSuffix();
                if (sSuffix == "pdf" ||
                        sFilePath.endsWith(QString(".pdf"))) {  //  打开第一个pdf文件
                    canOpenFileList.append(sFilePath);
                } else {
                    if (!noOpenFileList.contains(sSuffix)) {
                        noOpenFileList.append(sSuffix);
                    }
                }
            }
        }

        foreach (auto s, noOpenFileList) {
            QString msgType = s;
            if (msgType == "") {
                msgType = tr("Unknown type");
            }
            QString msgTitle = QString("%1").arg(msgType);
            QString msgContent = tr("%1 is not supported").arg(msgTitle);
            onShowTips(msgContent);
        }

        if (canOpenFileList.count() > 0) {
            QString sRes = "";

            foreach (auto s, canOpenFileList) {
                sRes += s + Constant::sQStringSep;
            }

            notifyMsg(MSG_OPEN_FILE_PATH_S, sRes);
        }
    }
}

void CentralWidget::initWidget()
{
    auto pStcakLayout = new QStackedLayout(this);
    pStcakLayout->setContentsMargins(0, 0, 0, 0);
    pStcakLayout->setSpacing(0);

    pStcakLayout->addWidget(new HomeWidget);

    auto pSplitter = new DSplitter;
    pSplitter->setHandleWidth(5);
    pSplitter->setChildrenCollapsible(false);  //  子部件不可拉伸到 0

    pSplitter->addWidget(new LeftSidebarWidget(this));
    pSplitter->addWidget(new DocShowShellWidget(this));

    QList<int> list_src;
    list_src.append(LEFTNORMALWIDTH);
    list_src.append(1000 - LEFTNORMALWIDTH);

    pSplitter->setSizes(list_src);

    pStcakLayout->addWidget(pSplitter);
}

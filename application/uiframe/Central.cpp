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

#include "Central.h"

#include <QFileInfo>
#include <QMimeData>
#include <QUrl>
#include <DMessageManager>
#include <QStackedLayout>

#include "business/AppInfo.h"
#include "utils/utils.h"
#include "app/ProcessController.h"
#include "CentralNavPage.h"
#include "CentralDocPage.h"

Central::Central(DWidget *parent)
    : CustomWidget(CENTRAL_WIDGET, parent)
{
    setAcceptDrops(true);

    m_pMsgList = {CENTRAL_SHOW_TIP, CENTRAL_INDEX_CHANGE};
    initWidget();
    initConnections();

    dApp->m_pModelService->addObserver(this);
}

Central::~Central()
{
    dApp->m_pModelService->removeObserver(this);
}

void Central::keyPressEvent(QKeyEvent *event)
{
    //  不是正常显示, 则是全屏模式或者幻灯片模式, 进行页面跳转
    QStringList pFilterList = QStringList() << KeyStr::g_pgup << KeyStr::g_pgdown
                              << KeyStr::g_down << KeyStr::g_up
                              << KeyStr::g_left << KeyStr::g_right << KeyStr::g_space;
    QString key = Utils::getKeyshortcut(event);
    if (pFilterList.contains(key)) {
        QJsonObject obj;
        obj.insert("type", "keyPress");
        obj.insert("key", key);

        QJsonDocument doc = QJsonDocument(obj);
        notifyMsg(E_APP_MSG_TYPE, doc.toJson(QJsonDocument::Compact));
    }

    CustomWidget::keyPressEvent(event);
}

void Central::initConnections()
{
}

void Central::OnSetCurrentIndex()
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        pLayout->setCurrentIndex(0);
    }
}

void Central::SlotOpenFiles(const QString &filePaths)
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        pLayout->setCurrentIndex(1);
    }

    notifyMsg(MSG_TAB_ADD, filePaths);
}

void Central::onFilesOpened()
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        pLayout->setCurrentIndex(1);
    }
}

void Central::onShowTip(const QString &contant)
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

int Central::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == CENTRAL_INDEX_CHANGE) {
        OnSetCurrentIndex();
    } else if (msgType == CENTRAL_SHOW_TIP) {
        onShowTip(msgContent);
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }
    return MSG_NO_OK;
}

void Central::dragEnterEvent(QDragEnterEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->accept();
    } else if (mimeData->hasFormat("reader/tabbar")) {
        event->accept();
    }
}

void Central::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasFormat("reader/tabbar")) {
        event->setDropAction(Qt::MoveAction);
        event->accept();

        QString filePath = mimeData->data("reader/filePath");
        SlotOpenFiles(filePath);

    } else if (mimeData->hasUrls()) {
        QStringList noOpenFileList;
        QStringList canOpenFileList;

        for (auto url : mimeData->urls()) {
            QString sFilePath = url.toLocalFile();

            QFileInfo file(sFilePath);
            if (file.isFile()) {
                QString sSuffix = file.completeSuffix();
                if (sSuffix == "pdf" || sFilePath.endsWith(QString(".pdf"))) {  //  打开第一个pdf文件
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
            onShowTip(msgContent);
        }

        if (canOpenFileList.count() > 0) {
            QString sRes = "";

            foreach (auto s, canOpenFileList) {
                if (!ProcessController::existFilePath(s))
                    sRes += s + Constant::sQStringSep;
            }

            SlotOpenFiles(sRes);
        }
    }
}

void Central::initWidget()
{
    auto pStcakLayout = new QStackedLayout(this);
    pStcakLayout->setContentsMargins(0, 0, 0, 0);
    pStcakLayout->setSpacing(0);

    CentralNavPage *nav = new CentralNavPage;
    connect(nav, SIGNAL(sigOpenFilePaths(const QString &)), SLOT(SlotOpenFiles(const QString &)));
    connect(nav, SIGNAL(filesOpened()), SLOT(onFilesOpened()));
    pStcakLayout->addWidget(nav);

    pStcakLayout->addWidget(new CentralDocPage);
}

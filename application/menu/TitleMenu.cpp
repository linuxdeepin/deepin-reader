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
#include "TitleMenu.h"

#include <QSignalMapper>
#include <QDesktopServices>
#include <QUrl>

#include "controller/DataManager.h"
#include "controller/NotifySubject.h"
#include "subjectObserver/MsgHeader.h"
#include "subjectObserver/ModuleHeader.h"

TitleMenu::TitleMenu(DWidget *parent)
    : DMenu(parent)
{
    __InitActions();
}

void TitleMenu::__InitActions()
{
    auto pSigManager = new QSignalMapper(this);
    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(slotActionTrigger(const QString &)));

    QStringList firstActionList = QStringList() << tr("Open") << tr("Save") << tr("Save as")
                                  << tr("Display in file manager") << tr("Print") << tr("Document info");
    QStringList firstActionObjList = QStringList() << "Open" << "Save" << "Save as"
                                     << "Display in file manager" << "Print" << "Document info";

    __CreateActionMap(pSigManager, firstActionList, firstActionObjList);

    QStringList secondActionList = QStringList() << tr("Search") /*<< tr("Fullscreen")*/ << tr("Slide show")
                                   << tr("Zoom in") << tr("Zoom out");
    QStringList secondActionObjList = QStringList() << "Search" /*<< "Fullscreen" */ << "Slide show"
                                      << "Zoom in" << "Zoom out";

    __CreateActionMap(pSigManager, secondActionList, secondActionObjList);

    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        if (a->objectName() == "Open") {
            a->setDisabled(false);
            break;
        }
    }
}

void TitleMenu::__CreateActionMap(QSignalMapper *pSigManager, const QStringList &actionList, const QStringList &actionObjList)
{
    int nFirstSize = actionList.size();
    for (int iLoop = 0; iLoop < nFirstSize; iLoop++) {
        QString sActionName = actionList.at(iLoop);
        QString sObjName = actionObjList.at(iLoop);

        QAction *_action = __CreateAction(sActionName, sObjName);
        connect(_action, SIGNAL(triggered()), pSigManager, SLOT(map()));
        pSigManager->setMapping(_action, sObjName);
    }
    this->addSeparator();
}

QAction *TitleMenu::__CreateAction(const QString &actionName, const QString &objName)
{
    QAction *action = new QAction(actionName, this);
    action->setObjectName(objName);
    action->setDisabled(true);
    this->addAction(action);
    this->addSeparator();

    return action;
}

void TitleMenu::__OpenFolder()
{
    QString strFilePath = DataManager::instance()->strOnlyFilePath();
    if (strFilePath != "") {
        int nLastPos = strFilePath.lastIndexOf('/');
        strFilePath = strFilePath.mid(0, nLastPos);
        strFilePath = QString("file://") + strFilePath;
        QDesktopServices::openUrl(QUrl(strFilePath));
    }
}

//  播放幻灯片, 因为需要将窗口最大化, 通过发送信号完成
void TitleMenu::__SlideShow()
{
    emit sigSetSlideShow();
}

void TitleMenu::notifyMsg(const int &iKey, const QString &sMsg)
{
    g_NotifySubject::getInstance()->notifyMsg(iKey, sMsg);
}

void TitleMenu::slotActionTrigger(const QString &sAction)
{
    if (sAction == "Open") {
        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_o);
    } else if (sAction == "Save") {
        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_s);
    } else if (sAction == "Save as") {
        notifyMsg(MSG_OPERATION_SAVE_AS_FILE);
    } else if (sAction == "Display in file manager") {
        __OpenFolder();
    } else if (sAction == "Print") {
        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_p);
    } else if (sAction == "Document info") {
        notifyMsg(MSG_OPERATION_ATTR);
    } else if (sAction == "Search") {
        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_f);
    } else if (sAction == "Fullscreen") {
        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_f11);
    } else if (sAction == "Slide show") {
        __SlideShow();
    } else if (sAction == "Zoom in") {
        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_larger);
    } else if (sAction == "Zoom out") {
        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_smaller);
    }
}

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

#include "controller/DataManager.h"
#include "controller/NotifySubject.h"
#include "subjectObserver/MsgHeader.h"
#include "subjectObserver/ModuleHeader.h"

TitleMenu::TitleMenu(DWidget *parent)
    : CustomMenu("TitleMenu", parent)
{
    initActions();

    m_pNotifySubject = g_NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

TitleMenu::~TitleMenu()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

int TitleMenu::dealWithData(const int &, const QString &)
{
    return 0;
}

void TitleMenu::initActions()
{
    auto pSigManager = new QSignalMapper(this);
    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(slotActionTrigger(const QString &)));

    QStringList firstActionList = QStringList() << tr("New window") << tr("New tab");
    QStringList firstActionObjList = QStringList() << "New window" << "New tab";
    __CreateActionMap(pSigManager, firstActionList, firstActionObjList);

    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        a->setDisabled(false);
    }

    QStringList secondActionList = QStringList() << tr("Save") << tr("Save as") << tr("Print") << tr("Slide show");
    QStringList secondActionObjList = QStringList() << "Save" << "Save as" << "Print" << "Slide show";
    __CreateActionMap(pSigManager, secondActionList, secondActionObjList);

    QStringList thirdActionList = QStringList() << tr("Magnifer") << tr("Document info") << tr("Display in file manager");
    QStringList thirdActionObjList = QStringList() << "Magnifer" << "Document info" << "Display in file manager";
    __CreateActionMap(pSigManager, thirdActionList, thirdActionObjList);
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

    return action;
}

//void TitleMenu::__OpenFolder()
//{
//    QString strFilePath = DataManager::instance()->strOnlyFilePath();
//    if (strFilePath != "") {
//        int nLastPos = strFilePath.lastIndexOf('/');
//        strFilePath = strFilePath.mid(0, nLastPos);
//        strFilePath = QString("file://") + strFilePath;
//        QDesktopServices::openUrl(QUrl(strFilePath));
//    }
//}

void TitleMenu::slotActionTrigger(const QString &sAction)
{
    if (sAction == "New window") {
        notifyMsg(MSG_MENU_NEW_WINDOW);
    } else if (sAction == "New tab") {
        notifyMsg(MSG_MENU_NEW_TAB);
    } else if (sAction == "Save") {
        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_s);
    } else if (sAction == "Save as") {
        notifyMsg(MSG_OPERATION_SAVE_AS_FILE);
    } else if (sAction == "Display in file manager") {
        notifyMsg(MSG_MENU_OPEN_FOLDER);
    } else if (sAction == "Print") {
        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_p);
    } else if (sAction == "Document info") {
        notifyMsg(MSG_OPERATION_ATTR);
    } else if (sAction == "Magnifer") {
        notifyMsg(MSG_MENU_MAGNIFER);
    } else if (sAction == "Slide show") {
        notifyMsg(MSG_OPERATION_SLIDE);
    }
//    else if (sAction == "Search") {
//        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_f);
//    } else if (sAction == "Fullscreen") {
//        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_f11);
//    }  else if (sAction == "Zoom in") {
//        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_larger);
//    } else if (sAction == "Zoom out") {
//        notifyMsg(MSG_NOTIFY_KEY_MSG, KeyStr::g_ctrl_smaller);
//    }
}

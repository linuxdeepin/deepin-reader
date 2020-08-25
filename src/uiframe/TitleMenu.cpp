/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     wangzhixuan<wangzhixuan@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#include "widgets/HandleMenu.h"
#include "DocSheet.h"

#include <QSignalMapper>
#include <QDebug>

TitleMenu::TitleMenu(DWidget *parent)
    : CustomMenu(parent)
{
    initActions();
}

void TitleMenu::onCurSheetChanged(DocSheet *sheet)
{
    if (nullptr == sheet || !sheet->isUnLocked()) {
        disableAllAction();
        return;
    }

    const QList<QAction *> &actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        a->setVisible(true);
        a->setDisabled(false);
    }
    m_handleMenu->setDisabled(false);
    m_handleMenu->readCurDocParam(sheet);
    disableSaveButton(!sheet->fileChanged());
}

void TitleMenu::disableAllAction()
{
    QStringList actiontextlist;
    actiontextlist << "Save" << "Save as" << "Display in file manager" << "Magnifer" << "Search";
    const QList<QAction *> &actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        if (actiontextlist.indexOf(a->objectName()) != -1)
            a->setDisabled(true);
    }
    m_handleMenu->setDisabled(true);
}

void TitleMenu::disableSaveButton(bool disable)
{
    const QList<QAction *> &actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        if (a->text() == tr("Save")) {
            a->setDisabled(disable);
            break;
        }
    }
}

void TitleMenu::initActions()
{
    auto pSigManager = new QSignalMapper(this);
    connect(pSigManager, SIGNAL(mapped(const QString &)), this, SIGNAL(sigActionTriggered(const QString &)));

    QStringList firstActionList = QStringList() << tr("New window") << tr("New tab");
    QStringList firstActionObjList = QStringList() << "New window" << "New tab";
    createActionMap(pSigManager, firstActionList, firstActionObjList);
    this->addSeparator();

    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        a->setDisabled(false);
    }

    QStringList secondActionList = QStringList() << tr("Save") << tr("Save as");
    QStringList secondActionObjList = QStringList() << "Save" << "Save as";
    createActionMap(pSigManager, secondActionList, secondActionObjList);
    this->addSeparator();

    QStringList thirdActionList = QStringList() << tr("Display in file manager") << tr("Magnifer");
    QStringList thirdActionObjList = QStringList() << "Display in file manager" << "Magnifer";
    createActionMap(pSigManager, thirdActionList, thirdActionObjList);

    m_handleMenu = new HandleMenu(this);
    m_handleMenu->setDisabled(true);
    m_handleMenu->setTitle(tr("Tools"));
    this->addMenu(m_handleMenu);

    QStringList fourActionList = QStringList() << tr("Search");
    QStringList fourActionObjList = QStringList() << "Search";
    createActionMap(pSigManager, fourActionList, fourActionObjList);
    this->addSeparator();
}

void TitleMenu::createActionMap(QSignalMapper *pSigManager, const QStringList &actionList, const QStringList &actionObjList)
{
    int nFirstSize = actionList.size();
    for (int iLoop = 0; iLoop < nFirstSize; iLoop++) {
        QString sActionName = actionList.at(iLoop);
        QString sObjName = actionObjList.at(iLoop);

        QAction *_action = createAction(sActionName, sObjName);
        connect(_action, SIGNAL(triggered()), pSigManager, SLOT(map()));
        pSigManager->setMapping(_action, sObjName);
    }
}

QAction *TitleMenu::createAction(const QString &actionName, const QString &objName)
{
    QAction *action = new QAction(actionName, this);
    action->setObjectName(objName);
    action->setDisabled(true);
    this->addAction(action);
    return action;
}

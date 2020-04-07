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
#include "DocSheet.h"

TitleMenu::TitleMenu(DWidget *parent)
    : CustomMenu(TITLE_MENU, parent)
{
    initActions();
}

void TitleMenu::onCurSheetChanged(DocSheet *sheet)
{
    if (nullptr == sheet) {
        disableAllAction();
        return;
    }

    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        a->setDisabled(false);
    }

    disableSaveButton(!sheet->qGetFileChange());
}

void TitleMenu::onCurSheetSaved(DocSheet *sheet)
{
    if (nullptr == sheet) {
        disableSaveButton(true);
        return;
    }

    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        a->setDisabled(false);
    }

    disableSaveButton(!sheet->qGetFileChange());
}

void TitleMenu::disableAllAction()
{
    QStringList actiontextlist;
    actiontextlist << "Save" << "Save as" << "Print" << "Slide show" << "Magnifer" << "Document info" << "Display in file manager";
    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        if (actiontextlist.indexOf(a->objectName()) != -1)
            a->setDisabled(true);
    }
}

void TitleMenu::disableSaveButton(bool disable)
{
    auto actions = this->findChildren<QAction *>();
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

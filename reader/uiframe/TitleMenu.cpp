// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TitleMenu.h"
#include "HandleMenu.h"
#include "DocSheet.h"
#include "Global.h"
#include "ddlog.h"

TitleMenu::TitleMenu(DWidget *parent)
    : DMenu(parent)
{
    qCDebug(appLog) << "Initializing TitleMenu...";
    QStringList firstActionList = QStringList() << tr("New window") << tr("New tab");
    QStringList firstActionObjList = QStringList() << "New window" << "New tab";
    createActionMap(firstActionList, firstActionObjList);
    this->addSeparator();

    auto actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        a->setDisabled(false);
    }

    QStringList secondActionList = QStringList() << tr("Save") << tr("Save as");
    QStringList secondActionObjList = QStringList() << "Save" << "Save as";
    createActionMap(secondActionList, secondActionObjList);
    this->addSeparator();

    QStringList thirdActionList = QStringList() << tr("Display in file manager") << tr("Magnifer");
    QStringList thirdActionObjList = QStringList() << "Display in file manager" << "Magnifer";
    createActionMap(thirdActionList, thirdActionObjList);

    m_handleMenu = new HandleMenu(this);
    m_handleMenu->setDisabled(true);
    m_handleMenu->setTitle(tr("Tools"));
    m_handleMenu->setAccessibleName("Menu_Hand");
    this->addMenu(m_handleMenu);

    QStringList fourActionList = QStringList() << tr("Search") << tr("Print");
    QStringList fourActionObjList = QStringList() << "Search" << "Print";
    createActionMap(fourActionList, fourActionObjList);
    this->addSeparator();
}

void TitleMenu::onCurSheetChanged(DocSheet *sheet)
{
    qCDebug(appLog) << "Current sheet changed";
    if (nullptr == sheet || !sheet->opened()) {
        qCWarning(appLog) << "Invalid sheet, disabling all actions";
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

    QAction *searchAction = this->findChild<QAction *>("Search");
    if (searchAction) {
        if (sheet->fileType() == Dr::PDF || sheet->fileType() == Dr::DOCX
#ifdef XPS_SUPPORT_ENABLED
            || sheet->fileType() == Dr::XPS
#endif
        ) {
            qCDebug(appLog) << "Enabling search for PDF/DOCX";
            searchAction->setVisible(true);
        } else {
            qCDebug(appLog) << "Disabling search for other formats";
            searchAction->setVisible(false);
        }
    }
    qCDebug(appLog) << "TitleMenu::onCurSheetChanged end";
}

void TitleMenu::onActionTriggered()
{
    qCDebug(appLog) << "Menu action triggered";
    QAction *action = static_cast<QAction *>(sender());
    if (nullptr == action) {
        qCWarning(appLog) << "Action triggered but sender is null";
        return;
    }

    qCDebug(appLog) << "Menu action triggered:" << action->objectName();
    emit sigActionTriggered(action->objectName());
}

void TitleMenu::disableAllAction()
{
    qCDebug(appLog) << "Disabling all menu actions";
    QStringList actiontextlist;
    actiontextlist << "Save" << "Save as" << "Display in file manager" << "Magnifer" << "Search" << "Print";
    const QList<QAction *> &actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        if (actiontextlist.indexOf(a->objectName()) != -1) {
            a->setDisabled(true);
            qCDebug(appLog) << "Disabled action:" << a->objectName();
        }
    }
    m_handleMenu->setDisabled(true);
    qCDebug(appLog) << "Disabled all menu actions";
}

void TitleMenu::disableSaveButton(bool disable)
{
    qCDebug(appLog) << (disable ? "Disabling" : "Enabling") << "Save button";
    const QList<QAction *> &actions = this->findChildren<QAction *>();
    foreach (QAction *a, actions) {
        if (a->text() == tr("Save")) {
            a->setDisabled(disable);
            qCDebug(appLog) << "Save button state:" << (disable ? "disabled" : "enabled");
            break;
        }
    }
    qCDebug(appLog) << "Save button state:" << (disable ? "disabled" : "enabled");
}

void TitleMenu::createActionMap(const QStringList &actionList, const QStringList &actionObjList)
{
    qCDebug(appLog) << "Creating action map";
    int nFirstSize = actionList.size();
    for (int iLoop = 0; iLoop < nFirstSize; iLoop++) {
        QString sActionName = actionList.at(iLoop);
        QString sObjName = actionObjList.at(iLoop);

        QAction *action = createAction(sActionName, sObjName);
        connect(action, SIGNAL(triggered()), this, SLOT(onActionTriggered()));
    }
    qCDebug(appLog) << "Created action map";
}

QAction *TitleMenu::createAction(const QString &actionName, const QString &objName)
{
    qCDebug(appLog) << "Creating action:" << actionName;
    QAction *action = new QAction(actionName, this);
    action->setObjectName(objName);
    action->setDisabled(true);
    this->addAction(action);
    qCDebug(appLog) << "Created action:" << actionName;
    return action;
}

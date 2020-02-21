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
#include "HandleMenu.h"

HandleMenu::HandleMenu(DWidget *parent)
    : CustomMenu("HandleMenu", parent)
{
    initActions();

    dApp->m_pModelService->addObserver(this);

}

HandleMenu::~HandleMenu()
{
    dApp->m_pModelService->removeObserver(this);
}

int HandleMenu::dealWithData(const int &, const QString &)
{
    return 0;
}

void HandleMenu::initActions()
{
    auto actionGroup = new QActionGroup(this);

    auto action = new QAction(tr("Select Text"), this);
    action->setObjectName("defaultshape");
    action->setCheckable(true);
    action->setChecked(true);

    actionGroup->addAction(action);
    addAction(action);
    addSeparator();

    action = new QAction(tr("Hand Tool"), this);
    action->setObjectName("handleshape");
    action->setCheckable(true);

    actionGroup->addAction(action);
    addAction(action);

    connect(actionGroup, SIGNAL(triggered(QAction *)), SLOT(SlotActionTrigger(QAction *)));
}

void HandleMenu::SlotActionTrigger(QAction *action)
{
    QString sAction = action->objectName();
    if (sAction != "") {
        emit sigCurrentTool(sAction);
    }
}

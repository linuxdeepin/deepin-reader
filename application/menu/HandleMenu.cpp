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
    : CustomMenu(HANDLE_MENU, parent)
{
    initActions();
}

void HandleMenu::initActions()
{
    auto actionGroup = new QActionGroup(this);

    auto action = new QAction(tr("Select Text"), this);
    connect(action, SIGNAL(triggered()), SLOT(slotSelectText()));
    action->setObjectName("defaultshape");
    action->setCheckable(true);
    action->setChecked(true);

    actionGroup->addAction(action);
    addAction(action);
    addSeparator();

    action = new QAction(tr("Hand Tool"), this);
    connect(action, SIGNAL(triggered()), SLOT(slotHandTool()));
    action->setObjectName("handleshape");
    action->setCheckable(true);

    actionGroup->addAction(action);
    addAction(action);
}

void HandleMenu::slotHandTool()
{
    emit sigClickAction(E_HANDLE_HANDLE_TOOL);
}

void HandleMenu::slotSelectText()
{
    emit sigClickAction(E_HANDLE_SELECT_TEXT);
}

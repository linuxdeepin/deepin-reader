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
#include <QDebug>

HandleMenu::HandleMenu(DWidget *parent)
    : CustomMenu(parent)
{
    initActions();
}

void HandleMenu::setHandShape(int index)
{
    if (1 == index)
        m_handAction->setChecked(true);
    else
        m_textAction->setChecked(true);
}

void HandleMenu::initActions()
{
    auto actionGroup = new QActionGroup(this);

    m_textAction = new QAction(tr("Select Text"), this);
    connect(m_textAction, SIGNAL(triggered()), SLOT(slotSelectText()));
    m_textAction->setObjectName("defaultshape");
    m_textAction->setCheckable(true);
    m_textAction->setChecked(true);

    actionGroup->addAction(m_textAction);
    addAction(m_textAction);

    m_handAction = new QAction(tr("Hand Tool"), this);
    connect(m_handAction, SIGNAL(triggered()), SLOT(slotHandTool()));
    m_handAction->setObjectName("handleshape");
    m_handAction->setCheckable(true);

    actionGroup->addAction(m_handAction);
    addAction(m_handAction);
}

void HandleMenu::slotHandTool()
{
    emit sigClickAction(E_HANDLE_HANDLE_TOOL);
}

void HandleMenu::slotSelectText()
{
    emit sigClickAction(E_HANDLE_SELECT_TEXT);
}

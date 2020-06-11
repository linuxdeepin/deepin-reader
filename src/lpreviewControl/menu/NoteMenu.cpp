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
#include "NoteMenu.h"

NoteMenu::NoteMenu(DWidget *parent)
    : CustomMenu(parent)
{
    initActions();
}

void NoteMenu::initActions()
{
    QAction *copyAction = this->addAction(tr("Copy"));
    connect(copyAction, SIGNAL(triggered()), this, SLOT(slotCopy()));

    QAction *delAction = this->addAction(tr("Remove annotation"));
    connect(delAction, SIGNAL(triggered()), this, SLOT(slotDelete()));

    QAction *delAllAction = this->addAction(tr("Remove All annotation"));
    connect(delAllAction, SIGNAL(triggered()), this, SLOT(slotAllDelete()));
}

void NoteMenu::slotCopy()
{
    emit sigClickAction(E_NOTE_COPY);
}

void NoteMenu::slotDelete()
{
    emit sigClickAction(E_NOTE_DELETE);
}

void NoteMenu::slotAllDelete()
{
    emit sigClickAction(E_NOTE_DELETE_ALL);
}

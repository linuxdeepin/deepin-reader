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
#include "BookMarkMenu.h"
#include <QDebug>

BookMarkMenu::BookMarkMenu(DWidget *parent)
    : CustomMenu(BOOKMARK_MENU, parent)
{
    initActions();
}

void BookMarkMenu::initActions()
{
    QAction *dltBookMarkAction = this->addAction(tr("Remove bookmark"));
    connect(dltBookMarkAction, SIGNAL(triggered()), SLOT(slotDelete()));
}

void BookMarkMenu::slotDelete()
{
    emit sigClickAction(E_BOOKMARK_DELETE);
}

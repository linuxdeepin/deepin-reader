/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Rekols    <rekols@foxmail.com>
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

#include "LineBar.h"
#include "utils/utils.h"

#include <QDebug>

LineBar::LineBar(DLineEdit *parent)
    : DLineEdit(parent)
{
    // Init.
    setClearButtonEnabled(true);

    connect(this, &DLineEdit::textChanged, this, &LineBar::handleTextChanged, Qt::QueuedConnection);
}
void LineBar::handleTextChanged()
{
    if (text() == "") {
        clearContent();
    }
}

void LineBar::keyPressEvent(QKeyEvent *e)
{
    QString key = Utils::getKeyshortcut(e);

    if (key == "Esc") {
        pressEsc();
    } else if (key == "Return" || key == "Enter") {
        pressEnter();
    } else if (key == "Ctrl + Return") {
        pressCtrlEnter();
    } else {
        // Pass event to DLineEdit continue, otherwise you can't type anything after here. ;)
        DLineEdit::keyPressEvent(e);
    }
}

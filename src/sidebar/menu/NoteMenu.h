/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#ifndef NOTEMENU_H
#define NOTEMENU_H

#include "widgets/CustomMenu.h"

class NoteMenu : public CustomMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(NoteMenu)

public:
    explicit NoteMenu(DWidget *parent = nullptr);

    // CustomMenu interface
protected:
    void initActions() override;

private slots:
    void slotCopy();
    void slotDelete();
    void slotAllDelete();
};

#endif // NOTEMENU_H

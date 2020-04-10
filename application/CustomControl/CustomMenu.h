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
#ifndef CUSTOMMENU_H
#define CUSTOMMENU_H

#include <DMenu>

#include <QJsonObject>
#include <QJsonDocument>

#include "application.h"

DWIDGET_USE_NAMESPACE

enum E_MENU_ACTION {
    E_HANDLE_SELECT_TEXT,
    E_HANDLE_HANDLE_TOOL,
    E_BOOKMARK_DELETE,
    E_NOTE_COPY,
    E_NOTE_DELETE
};

class CustomMenu : public DMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(CustomMenu)

public:
    CustomMenu(DWidget *parent = nullptr);

signals:
    void sigClickAction(const int &);
    void sigActionTrigger(const int &, const QString &);

public:
    virtual int dealWithData(const int &, const QString &);

protected:
    virtual void initActions() = 0;
};


#endif // CUSTOMMENU_H

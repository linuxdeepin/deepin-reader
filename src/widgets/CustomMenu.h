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
#ifndef CUSTOMMENU_H
#define CUSTOMMENU_H

#include <DMenu>

DWIDGET_USE_NAMESPACE

typedef enum E_MENU_ACTION {
    E_HANDLE_SELECT_TEXT,
    E_HANDLE_HANDLE_TOOL,
    E_BOOKMARK_DELETE,
    E_BOOKMARK_DELETE_ALL,
    E_NOTE_COPY,
    E_NOTE_DELETE,
    E_NOTE_DELETE_ALL
} E_MENU_ACTION;

typedef struct MenuItem_t {
    QString rootItem;
    QList<MenuItem_t> childItems;

    MenuItem_t(const QString &rootitem)
    {
        this->rootItem = rootitem;
    }
} MenuItem_t;

/**
 * @brief The CustomMenu class
 * 菜单
 */
class CustomMenu : public DMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(CustomMenu)

public:
    CustomMenu(DWidget *parent = nullptr);
    virtual ~CustomMenu();

signals:
    /**
     * @brief sigClickAction
     * Aciton点击信号
     */
    void sigClickAction(const int &);

public:
    /**
     * @brief createAction
     * 创建Action
     * @param objName
     * @param member
     * @param checkable
     * @return
     */
    QAction *createAction(const QString &objName, const char *member, bool checkable);

protected:
    virtual void initActions() {}
};


#endif // CUSTOMMENU_H

/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     wangzhixuan<wangzhixuan@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#ifndef TITLEMENU_H
#define TITLEMENU_H

#include <DMenu>

class DocSheet;
class HandleMenu;

/**
 * @brief The TitleMenu class
 * 窗体默认菜单
 */
DWIDGET_USE_NAMESPACE
class TitleMenu : public DMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(TitleMenu)

public:
    explicit TitleMenu(DWidget *parent = nullptr);

signals:
    /**
     * @brief sigActionTriggered
     * 菜单单击事件
     * @param action 某个菜单名称
     */
    void sigActionTriggered(const QString &action);

public slots:
    /**
     * @brief onCurSheetChanged
     * 文档窗口切换
     */
    void onCurSheetChanged(DocSheet *);

    /**
     * @brief onActionTriggered
     * 触发
     */
    void onActionTriggered();

public:
    /**
     * @brief disableAllAction
     * 所有事件皆不可用
     */
    void disableAllAction();

private:
    /**
     * @brief disableSaveButton
     * 设置保存按钮是否可用
     * @param disable
     */
    void disableSaveButton(bool disable);

private:
    /**
     * @brief createActionMap
     * 创建菜单事件集合
     * @param actionList
     * @param actionObjList
     */
    void createActionMap(const QStringList &actionList, const QStringList &actionObjList);

    /**
     * @brief createAction
     * 创建菜单事件
     * @param actionName 事件名称
     * @return 事件
     */
    QAction *createAction(const QString &actionName, const QString &);

private:
    HandleMenu *m_handleMenu = nullptr;
};


#endif // TITLEMENU_H

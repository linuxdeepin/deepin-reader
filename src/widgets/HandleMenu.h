/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
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
#ifndef HANDLEMENU_H
#define HANDLEMENU_H

#include <DMenu>

class DocSheet;
/**
 * @brief The HandleMenu class
 * 选择工具菜单
 */
DWIDGET_USE_NAMESPACE
class HandleMenu : public DMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(HandleMenu)

public:
    explicit HandleMenu(DWidget *parent = nullptr);

    /**
     * @brief readCurDocParam
     * 读取文档数据
     * @param docSheet
     */
    void readCurDocParam(DocSheet *docSheet);

protected:
    /**
     * @brief initActions
     * 初始化Action
     */
    void initActions();

private slots:
    /**
     * @brief onHandTool
     * 手型工具
     */
    void onHandTool();

    /**
     * @brief onSelectText
     * 文本选择工具
     */
    void onSelectText();

private:
    QAction *m_textAction = nullptr;
    QAction *m_handAction = nullptr;
    DocSheet *m_docSheet = nullptr;
};

#endif // HANDLEMENU_H

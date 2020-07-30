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

#include "widgets/CustomMenu.h"

class DocSheet;
class HandleMenu : public CustomMenu
{
    Q_OBJECT
    Q_DISABLE_COPY(HandleMenu)

public:
    HandleMenu(DWidget *parent = nullptr);
    void readCurDocParam(DocSheet *docSheet);

protected:
    void initActions();

private slots:
    void onHandTool();
    void onSelectText();

private:
    QAction *m_textAction;
    QAction *m_handAction;
    DocSheet *m_docSheet;
};

#endif // HANDLEMENU_H

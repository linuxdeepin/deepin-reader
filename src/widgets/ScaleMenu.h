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
#ifndef SCALEMENU_H
#define SCALEMENU_H

#include "CustomControl/CustomMenu.h"

class DocSheet;
class ScaleMenu : public CustomMenu
{
    Q_OBJECT

public:
    explicit ScaleMenu(QWidget *parent = nullptr);
    void readCurDocParam(DocSheet *docSheet);

private slots:
    void onTwoPage();
    void onFiteH();
    void onFiteW();
    void onDefaultPage();
    void onFitPage();
    void onScaleFactor();

private:
    QAction *m_pTwoPageAction;
    QAction *m_pFitDefaultAction;
    QAction *m_pFitWorHAction;
    QAction *m_pFiteHAction;
    QAction *m_pFiteWAction;
    DocSheet *m_sheet = nullptr;
    QList<QAction *> m_actionGroup;
};

#endif // SCALEMENU_H

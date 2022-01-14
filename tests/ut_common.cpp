/*
* Copyright (C) 2019 ~ 2021 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
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

#include "ut_common.h"

#include <DMenu>

#include <QProcess>

DWIDGET_USE_NAMESPACE

bool g_QWidget_isVisible_result = false;;               // QWidget isVisible返回值

UTCommon::UTCommon()
{

}

UTCommon::~UTCommon()
{

}

int qWidget_isVisible_stub()
{
    return g_QWidget_isVisible_result;
}

QAction *dmenu_exec_stub(const QPoint &, QAction *)
{
    return nullptr;
}

bool qProcess_startDetached_stub(const QString &, const QStringList &)
{
    return true;
}

void UTCommon::stub_DMenu_exec(Stub &stub)
{
    stub.set((QAction * (DMenu::*)(const QPoint &, QAction * at))ADDR(DMenu, exec), dmenu_exec_stub);
}

void UTCommon::stub_QWidget_isVisible(Stub &stub, bool isVisible)
{
    g_QWidget_isVisible_result = isVisible;
    stub.set(ADDR(QWidget, isVisible), qWidget_isVisible_stub);
}

void UTCommon::stub_QProcess_startDetached(Stub &stub)
{
#if !defined(Q_QDOC)
    stub.set((bool(*)(const QString &, const QStringList &))ADDR(QProcess, startDetached), qProcess_startDetached_stub);
#endif
}

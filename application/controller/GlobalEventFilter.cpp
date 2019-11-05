/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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
#include "GlobalEventFilter.h"
#include <QDebug>
#include <QEvent>
#include <QWindowStateChangeEvent>
#include "utils/utils.h"
#include "subjectObserver/ModuleHeader.h"

#include "NotifySubject.h"
#include "subjectObserver/MsgHeader.h"

GlobalEventFilter::GlobalEventFilter(QObject *parent)
    : QObject(parent)
{
    m_pFilterList = QStringList() << "Esc" << "F1" << "Up" << "Down" << "Ctrl+S" << "Ctrl+O" << "Ctrl+F" << "Del" << "PgUp" << "PgDown";
}

bool GlobalEventFilter::eventFilter(QObject *obj, QEvent *e)
{
    Q_UNUSED(obj)

    int nType = e->type();
    if (nType == QEvent::KeyPress) {
        QKeyEvent *event = static_cast<QKeyEvent *>(e);
        QString key = Utils::getKeyshortcut(event);
        if (m_pFilterList.contains(key)) {
            NotifySubject::getInstance()->sendMsg(MSG_NOTIFY_KEY_MSG, key);
            return true;
        }
    }

    return false;
}

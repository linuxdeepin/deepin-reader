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
#ifndef CUSTOMWIDGETDATA_H
#define CUSTOMWIDGETDATA_H

#include <QObject>
#include <QList>

class CustomWidget;

class CustomWidgetPrivate
{
protected:
    virtual ~CustomWidgetPrivate() {}

public:
    CustomWidgetPrivate();

    QList<int> getMsgList() const;
    QList<QString> getKeyMsgList() const;

public:
    CustomWidget     *q_ptr = nullptr;

protected:
    QList<int>          m_pMsgList;         //  需要处理的消息列表
    QList<QString>      m_pKeyMsgList;      //  需要处理的按键消息列表
};

#endif // CUSTOMWIDGETDATA_H

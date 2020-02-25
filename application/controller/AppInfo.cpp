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

#include "AppInfo.h"

AppInfo::AppInfo(QObject *parent)
    : QObject(parent)
{
    InitColor();
    InitKeyList();
}

void AppInfo::InitColor()
{
    m_listColor.append(QColor("#FFA503"));
    m_listColor.append(QColor("#FF1C49"));
    m_listColor.append(QColor("#9023FC"));
    m_listColor.append(QColor("#3468FF"));
    m_listColor.append(QColor("#00C7E1"));
    m_listColor.append(QColor("#05EA6B"));
    m_listColor.append(QColor("#FEF144"));
    m_listColor.append(QColor("#D5D5D1"));

    m_selectColor = m_listColor.at(0);
}

void AppInfo::InitKeyList()
{
//    m_pKeyList.append(QKeySequence::HelpContents);            //  dtk 已实现
//    m_pKeyList.append(QKeySequence::Delete);                  //  2020.2.12  delete 不能作快捷键使用
    m_pKeyList.append(QKeySequence::Find);
    m_pKeyList.append(QKeySequence::Open);
    m_pKeyList.append(QKeySequence::Print);
    m_pKeyList.append(QKeySequence::Save);
    m_pKeyList.append(QKeySequence::Copy);

    m_pKeyList.append(QKeySequence(Qt::Key_Space));
//    m_pKeyList.append(QKeySequence(Qt::Key_F11));             //  全屏暂时取消  2020.1.6    wzx
    m_pKeyList.append(QKeySequence(Qt::Key_Escape));

    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_1));
    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_2));
    m_pKeyList.append(QKeySequence(Qt::ALT | Qt::Key_Z));

    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_1));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_2));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_3));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_B));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_E));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_H));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_I));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_L));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_M));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_R));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_Minus));      //  ctrl+-
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_Equal));      //  ctrl+=

    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::Key_Plus));       //  ctrl++

    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_R));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_S));
    m_pKeyList.append(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_Slash));
}

QColor AppInfo::selectColor() const
{
    return m_selectColor;
}

void AppInfo::setSelectColor(const QColor &color)
{
    m_selectColor = color;
}

QList<QColor> AppInfo::getLightColorList()
{
    return m_listColor;
}

QList<QKeySequence> AppInfo::getKeyList() const
{
    return m_pKeyList;
}

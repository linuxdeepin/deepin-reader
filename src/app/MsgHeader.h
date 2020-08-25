/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
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
#ifndef MSGHEADER_H
#define MSGHEADER_H

#include <QString>

/**
 *  消息类型 定义
 */

namespace {

//  搜索框消息
enum E_FIND_WIDGET_MSG {
    E_FIND_PREV,
    E_FIND_NEXT,
    E_FIND_EXIT,
    E_FIND_CONTENT,
};

//  高亮&注释消息
enum E_NOTE_MSG {
    MSG_NOTE_ADD = 0,
    MSG_NOTE_DELETE,
    MSG_ALL_NOTE_DELETE
};
}


#endif  // MSGHEADER_H

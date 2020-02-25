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
#ifndef FILEDATA_H
#define FILEDATA_H

#include <QString>

struct FileData {
    bool        bIsChane = false;
    QString     sScale = "";
    bool        bLeftBarIsShow = false;
    bool        bIsOpen = false;            //  文档是否已打开
//    DocType_EM  m_nCurDocType = DocType_NULL;
};

#endif // FILEDATA_H

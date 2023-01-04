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
#ifndef UT_COMMON_H
#define UT_COMMON_H

#include "stub.h"

extern bool g_QWidget_isVisible_result;                 // QWidget isVisible返回值

class UTCommon
{
public:
    UTCommon();
    ~UTCommon();

    /**
     * @brief stub_DMenu_exec     针对DMenu的exec打桩
     * @param stub
     * @return
     */
    static void stub_DMenu_exec(Stub &stub);

    /**
     * @brief stub_QWidget_isVisible     针对QWidget的isVisible打桩
     * @param stub
     * @param isVisible
     */
    static void stub_QWidget_isVisible(Stub &stub, bool isVisible);

    /**
     * @brief stub_QProcess_startDetached     针对QProcess的startDetached打桩
     * @param stub
     */
    static void stub_QProcess_startDetached(Stub &stub);
};


#endif // UT_COMMON_H

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
#ifndef GLOBAL_H
#define GLOBAL_H

#include "DebugTimeManager.h"

#include <QString>

namespace Dr {

/**
启动时间                POINT-01
关闭时间                POINT-02
打开文档响应时间         POINT-03    文档名，文档大小
保存文档响应时间         POINT-04    文档名，文档大小
文管中双击文件打开文档时间 POINT-05    文档名，文档大小
**/

#ifdef PERF_ON
#define PERF_PRINT_BEGIN(point, desc) DebugTimeManager::getInstance()->beginPointLinux(point, desc)
#define PERF_PRINT_END(point, desc) DebugTimeManager::getInstance()->endPointLinux(point, desc)
#else
#define PERF_PRINT_BEGIN(point, desc)
#define PERF_PRINT_END(point, desc)
#endif

enum FileType {
    Unknown = 0,
    PDF,
    DJVU,
    DOCX,
    PS,
    DOC,
    PPTX
};
FileType fileType(const QString &filePath);

enum Rotation {
    RotateBy0 = 0,
    RotateBy90 = 1,
    RotateBy180 = 2,
    RotateBy270 = 3,
    NumberOfRotations = 4
};

enum MouseShape {
    MouseShapeNormal = 0,
    MouseShapeHand = 1,
    NumberOfMouseShapes = 2
};

enum ShowMode {
    DefaultMode     = 0,
    SliderMode      = 1,
    MagniferMode    = 2,
    FullScreenMode  = 3,
    NumberOfShowModes = 4
};

enum LayoutMode {
    SinglePageMode      = 0,
    TwoPagesMode        = 1,
    NumberOfLayoutModes = 2
};

enum ScaleMode {
    ScaleFactorMode     = 0,
    FitToPageWidthMode  = 1,
    FitToPageHeightMode = 2,
    NumberOfScaleModes  = 3,
    FitToPageDefaultMode = 4,
    FitToPageWorHMode   = 5
};

const QString key_up = "Up";
const QString key_down = "Down";
const QString key_left = "Left";
const QString key_right = "Right";
const QString key_space = "Space";            //空格用于停止启动幻灯片播放
const QString key_pgUp = "PgUp";              //  上一页
const QString key_pgDown = "PgDown";          //  下一页
const QString key_delete = "Del";             //删除
const QString key_esc = "Esc";                //  退出全屏\退出放映\退出放大镜
const QString key_f1   = "F1";                //  帮助
const QString key_f5   = "F5";                //    播放幻灯片
const QString key_f11  = "F11";               //  全屏
const QString key_ctrl_1 = "Ctrl+1";          //  适合页面状态
const QString key_ctrl_2 = "Ctrl+2";          //  适合高度
const QString key_ctrl_3 = "Ctrl+3";          //  适合宽度
const QString key_ctrl_d = "Ctrl+D";          //    添加书签
const QString key_ctrl_f = "Ctrl+F";          //  搜索
const QString key_ctrl_o = "Ctrl+O";          //  打开
const QString key_ctrl_e = "Ctrl+E";          //  导出
const QString key_ctrl_p = "Ctrl+P";          //  打印
const QString key_ctrl_s = "Ctrl+S";          //  保存
const QString key_ctrl_m = "Ctrl+M";          //  打开目标缩略图
const QString key_ctrl_r = "Ctrl+R";          //  左旋转
const QString key_ctrl_c = "Ctrl+C";
const QString key_ctrl_x = "Ctrl+X";
const QString key_ctrl_v = "Ctrl+V";
const QString key_ctrl_z = "Ctrl+Z";
const QString key_ctrl_a = "Ctrl+A";
const QString key_ctrl_equal = "Ctrl+=";        //  放大
const QString key_ctrl_smaller = "Ctrl+-";      //  缩小
const QString key_ctrl_wheel = "Ctrl+wheel";    //  ctrl+滚轮放大缩小
const QString key_alt_1 = "Alt+1";        //    选择工具
const QString key_alt_2 = "Alt+2";        //    手型工具
const QString key_alt_a = "Alt+A";        //    添加注释
const QString key_alt_h = "Alt+H";        //    添加高亮
const QString key_alt_z = "Alt+Z";        //    放大镜
const QString key_alt_f4 = "Alt+F4";      //    退出应用程序
const QString key_alt_harger = "Ctrl++";          //  放大
const QString key_ctrl_shift_r = "Ctrl+Shift+R";  //  右旋转
const QString key_ctrl_shift_s = "Ctrl+Shift+S";  //  另存为
const QString key_ctrl_shift_slash = "Ctrl+Shift+/";
}
#endif // GLOBAL_H

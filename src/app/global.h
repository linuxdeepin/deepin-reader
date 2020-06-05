/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* Central(NaviPage ViewPage)
*
* CentralNavPage(openfile)
*
* CentralDocPage(DocTabbar DocSheets)
*
* DocSheet(SheetSidebar SheetBrowser document)
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

#include <QString>

//deepin-reader

namespace Dr {

enum FileType {
    Unknown = 0,
    PDF = 1,
    PS = 2,
    DjVu = 3
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
    NumberOfScaleModes  = 3
};

}
#endif // GLOBAL_H

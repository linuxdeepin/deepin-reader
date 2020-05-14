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

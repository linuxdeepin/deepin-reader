#ifndef GLOBAL_H
#define GLOBAL_H

#include <QtGlobal>
#include <qnumeric.h>


enum Rotation
{
    RotateBy0 = 0,
    RotateBy90 = 1,
    RotateBy180 = 2,
    RotateBy270 = 3,
    NumberOfRotations = 4
};

enum RubberBandMode
{
    ModifiersMode = 0,
    CopyToClipboardMode = 1,
    AddAnnotationMode = 2,
    ZoomToSelectionMode = 3,
    NumberOfRubberBandModes = 4
};

enum LayoutMode
{
    SinglePageMode = 0,
    TwoPagesMode = 1,
    TwoPagesWithCoverPageMode = 2,
    MultiplePagesMode = 3,
    NumberOfLayoutModes = 4
};

enum ScaleMode
{
    ScaleFactorMode = 0,
    FitToPageWidthMode = 1,
    FitToPageSizeMode = 2,
    NumberOfScaleModes = 3
};

enum CompositionMode
{
    DefaultCompositionMode = 0,
    DarkenWithPaperColorMode = 1,
    LightenWithPaperColorMode = 2,
    NumberOfCompositionModes = 3
};



#endif // GLOBAL_H

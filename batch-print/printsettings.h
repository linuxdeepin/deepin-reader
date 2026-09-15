// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PRINTSETTINGS_H
#define PRINTSETTINGS_H

#include <QString>
#include <QStringList>

enum class Orientation {
    Portrait  = 0,
    Landscape = 1,
    Auto      = 2
};

enum class ColorMode {
    Auto  = 0,
    Color = 1,
    Gray  = 2
};

enum class DuplexMode {
    OneSided  = 0,
    LongEdge  = 1,
    ShortEdge = 2
};

enum class WatermarkLayout {
    Tiled  = 0,
    Center = 1
};

enum class WatermarkTextType {
    None          = 0,
    Confidential  = 1,
    Draft         = 2,
    Custom        = 3
};

struct PrintSettings {
    int copies = 1;
    DuplexMode duplex = DuplexMode::OneSided;
    ColorMode colorMode = ColorMode::Auto;

    Orientation orientation = Orientation::Auto;
    QString paperSize;

    bool watermarkEnabled = false;
    WatermarkLayout watermarkLayout = WatermarkLayout::Center;
    WatermarkTextType watermarkTextType = WatermarkTextType::Confidential;
    int watermarkAngle = 30;
    int watermarkSize = 100;
    int watermarkOpacity = 30;
    QString watermarkColor = QStringLiteral("#6F6F6F");
    QString watermarkText;

    QString pageRange;
    QString margin;
    int scale = 100;
    int perPage = 1;
    int printOrder = 0;
};

QStringList toCupsOptions(const PrintSettings &settings, bool printerSupportsColor);

#endif // PRINTSETTINGS_H

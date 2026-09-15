// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "printsettings.h"

#include <QGuiApplication>

QStringList toCupsOptions(const PrintSettings &settings, bool printerSupportsColor)
{
    QStringList options;

    int copies = qBound(1, settings.copies, 999);
    options << QStringLiteral("copies") << QString::number(copies);

    switch (settings.duplex) {
    case DuplexMode::OneSided:
        options << QStringLiteral("sides") << QStringLiteral("one-sided");
        break;
    case DuplexMode::LongEdge:
        options << QStringLiteral("sides") << QStringLiteral("two-sided-long-edge");
        break;
    case DuplexMode::ShortEdge:
        options << QStringLiteral("sides") << QStringLiteral("two-sided-short-edge");
        break;
    }

    bool useColor = false;
    switch (settings.colorMode) {
    case ColorMode::Auto:
        useColor = printerSupportsColor;
        break;
    case ColorMode::Color:
        useColor = true;
        break;
    case ColorMode::Gray:
        useColor = false;
        break;
    }
    options << QStringLiteral("ColorModel") << (useColor ? QStringLiteral("RGB") : QStringLiteral("Gray"));

    return options;
}

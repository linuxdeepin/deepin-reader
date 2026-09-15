// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FORMATCONVERTER_H
#define FORMATCONVERTER_H

#include <QString>
#include <QSizeF>

class FormatConverter
{
public:
    static bool convertToPdf(const QString &filePath, const QString &outputDir,
                             QString &outputPdfPath, QString &errorMsg);

    static QSizeF computePageSizeMm(double widthPx, double heightPx, int dpi);
};

#endif // FORMATCONVERTER_H

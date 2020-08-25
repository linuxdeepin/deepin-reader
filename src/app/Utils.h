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
#ifndef UTILS_H
#define UTILS_H

#include "Application.h"

#include <QKeyEvent>
#include <QObject>
#include <QPainter>
#include <QString>
#include <QIcon>

class Utils : public QObject
{
    Q_OBJECT

public:
    static QString getKeyshortcut(QKeyEvent *keyEvent);

    static bool fileExists(const QString &path);

    static QImage dropShadow(const QPixmap &px, qreal radius, const QColor &color);

    static QString getInputDataSize(const qint64 &);

    static QPixmap roundQPixmap(const QPixmap &img_in, int radius);

    static void copyText(const QString &sText);

    static QString getUuid();

    static QString getElidedText(const QFontMetrics &fontMetrics, const QSize &size, const QString &text, Qt::Alignment alignment);

    static bool copyFile(const QString &sourcePath, const QString &destinationPath);

    static QImage copyImage(const QImage &srcimg, int x, int y, int w, int h);

    static QPixmap renderSVG(const QString &filePath, const QSize &size);

    static QList<QColor> getHiglightColorList();

    static QColor getCurHiglightColor();

    static void setHiglightColorIndex(int index);

private:
    static int m_colorIndex;
};

#endif // UTILS_H

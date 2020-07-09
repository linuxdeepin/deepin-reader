/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Rekols    <rekols@foxmail.com>
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

#include "Utils.h"
#include <QMimeDatabase>
#include <QPainter>
#include <QStandardPaths>
#include <QPixmap>
#include <QClipboard>
#include <DApplication>
#include <QUuid>
#include <QFileInfo>



QT_BEGIN_NAMESPACE
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

bool Utils::fileExists(const QString &path)
{
    QFileInfo check_file(path);
    return check_file.exists() && check_file.isFile();
}

QString Utils::getKeyshortcut(QKeyEvent *keyEvent)
{
    QStringList keys;
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
    if (modifiers != Qt::NoModifier) {
        if (modifiers.testFlag(Qt::ControlModifier)) {
            keys.append("Ctrl");
        }

        if (modifiers.testFlag(Qt::AltModifier)) {
            keys.append("Alt");
        }

        if (modifiers.testFlag(Qt::MetaModifier)) {
            keys.append("Meta");
        }

        if (modifiers.testFlag(Qt::ShiftModifier)) {
            keys.append("Shift");
        }
    }

    if (keyEvent->key() != 0 && keyEvent->key() != Qt::Key_unknown) {
        keys.append(QKeySequence(keyEvent->key()).toString());
    }

    return keys.join("+");
}

QImage Utils::dropShadow(const QPixmap &px, qreal radius, const QColor &color)
{
    if (px.isNull()) {
        return QImage();
    }

    QImage tmp(px.size() * px.devicePixelRatio() + QSize(static_cast<int>(radius * 2), static_cast<int>(radius * 2)), QImage::Format_ARGB32_Premultiplied);
    tmp.setDevicePixelRatio(px.devicePixelRatio());
    tmp.fill(0);
    QPainter tmpPainter(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_Source);
    tmpPainter.drawPixmap(QPoint(static_cast<int>(radius), static_cast<int>(radius)), px);
    tmpPainter.end();

    // Blur the alpha channel.
    QImage blurred(tmp.size() * px.devicePixelRatio(), QImage::Format_ARGB32_Premultiplied);
    blurred.setDevicePixelRatio(px.devicePixelRatio());
    blurred.fill(0);
    QPainter blurPainter(&blurred);
    qt_blurImage(&blurPainter, tmp, radius, false, true);
    blurPainter.end();
    if (color == QColor(Qt::black)) {
        return blurred;
    }
    tmp = blurred;

    // Blacken the image...
    tmpPainter.begin(&tmp);
    tmpPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    tmpPainter.fillRect(tmp.rect(), color);
    tmpPainter.end();

    return tmp;
}

QString Utils::getInputDataSize(const qint64 &dSize)
{
    if (dSize < 1024) {
        return QString("%1 B").arg(dSize);
    }
    if (dSize < 1024 * 1024) {
        double d = dSize / 1024.0;
        return QString::number(d, 'f', 1) + " KB";
    }
    if (dSize < 1024 * 1024 * 1024) {
        double d = dSize / 1024.0 / 1024.0;
        return QString::number(d, 'f', 1) + " MB";
    }
    double d = dSize / 1024.0 / 1024.0 / 1024.0;
    return QString::number(d, 'f', 1) + " GB";
}

QPixmap Utils::roundQPixmap(const QPixmap &img_in, int radius)
{
    if (img_in.isNull()) {
        return QPixmap();
    }
    const QSize &imgSize = img_in.size();
    QPixmap pixmap(imgSize);
    pixmap.setDevicePixelRatio(dApp->devicePixelRatio());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPainterPath clippath;
    clippath.addRoundedRect(0, 0, imgSize.width() / dApp->devicePixelRatio(), imgSize.height() / dApp->devicePixelRatio(), radius, radius);
    painter.setClipPath(clippath);
    painter.drawPixmap(0, 0, img_in);
    return pixmap;
}

//  复制文字
void  Utils::copyText(const QString &sText)
{
    QClipboard *clipboard = DApplication::clipboard();  //获取系统剪贴板指针
    QString sOldText = clipboard->text(QClipboard::Clipboard);
    if (sOldText != sText) {
        clipboard->setText(sText);
    }
}

QString Utils::getUuid()
{
    const QString &uuid = QUuid::createUuid().toString();
    return  uuid.mid(1, uuid.length() - 2);
}

QString Utils::getElidedText(const QFontMetrics &fontMetrics, const QSize &size, const QString &text, Qt::Alignment alignment)
{
    int textLength = text.length();
    QString tmptext;
    for (int index = 0; index < textLength; index++) {
        const QString &ltext = text.left(index + 1);
        if (fontMetrics.boundingRect(0, 0, size.width(), size.height(), static_cast<int>(alignment | Qt::TextWrapAnywhere), ltext + QString("...")).height() > size.height()) {
            break;
        }
        tmptext = ltext;
    }
    if (tmptext.length() < textLength)
        tmptext.append("...");
    return tmptext;
}

bool Utils::copyFile(const QString &sourcePath, const QString &destinationPath)
{
    QFile sourceFile(sourcePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QFile destinationFile(destinationPath);
    if (!destinationFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        sourceFile.close();
        return false;
    }

    const qint64 maxSize = 4096;
    qint64 size = -1;

    QScopedArrayPointer< char > buffer(new char[maxSize]);

    int ret = true;
    do {
        if ((size = sourceFile.read(buffer.data(), maxSize)) < 0) {
            ret = false;
            break;
        }

        if (destinationFile.write(buffer.data(), size) < 0) {
            ret = false;
            break;
        }
    } while (size > 0);

    sourceFile.close();
    destinationFile.close();
    return ret;
}

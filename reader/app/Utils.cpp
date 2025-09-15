// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Utils.h"
#include "Application.h"

#include <QMimeDatabase>
#include <QPainter>
#include <QStandardPaths>
#include <QPixmap>
#include <QClipboard>
#include <QUuid>
#include <QFileInfo>
#include <QImageReader>
#include <QWidget>
#include <QProcessEnvironment>
#include <QPainterPath>

#include <unistd.h>
extern "C" {
#include "load_libs.h"
}
QT_BEGIN_NAMESPACE
extern Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

int Utils::m_colorIndex = 0;
QString Utils::m_currenFilePath = "";
QString Utils::getKeyshortcut(QKeyEvent *keyEvent)
{
    // qDebug() << "Processing keyboard shortcut";
    QStringList keys;
    Qt::KeyboardModifiers modifiers = keyEvent->modifiers();
    if (modifiers != Qt::NoModifier) {
        // qDebug() << "Utils::getKeyshortcut() - Processing modifiers";
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
        // qDebug() << "Utils::getKeyshortcut() - Adding key:" << keyEvent->key();
        keys.append(QKeySequence(keyEvent->key()).toString());
    }

    QString result = keys.join("+");
    // qDebug() << "Utils::getKeyshortcut() - Generated shortcut:" << result;
    return result;
}

QString Utils::getInputDataSize(const qint64 &dSize)
{
    qDebug() << "Formatting data size:" << dSize;
    if (dSize < 1024) {
        qDebug() << "Utils::getInputDataSize() - Size in bytes";
        return QString("%1 B").arg(dSize);
    }
    if (dSize < 1024 * 1024) {
        qDebug() << "Utils::getInputDataSize() - Size in KB";
        double d = dSize / 1024.0;
        return QString::number(d, 'f', 1) + " KB";
    }
    if (dSize < 1024 * 1024 * 1024) {
        qDebug() << "Utils::getInputDataSize() - Size in MB";
        double d = dSize / 1024.0 / 1024.0;
        return QString::number(d, 'f', 1) + " MB";
    }
    qDebug() << "Utils::getInputDataSize() - Size in GB";
    double d = dSize / 1024.0 / 1024.0 / 1024.0;
    return QString::number(d, 'f', 1) + " GB";
}

QPixmap Utils::roundQPixmap(const QPixmap &img_in, int radius)
{
    qDebug() << "Utils::roundQPixmap() - Starting pixmap rounding with radius:" << radius;
    if (img_in.isNull()) {
        qDebug() << "Utils::roundQPixmap() - Input pixmap is null, returning empty pixmap";
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
    qDebug() << "Utils::roundQPixmap() - Pixmap rounding completed";
    return pixmap;
}

void  Utils::copyText(const QString &sText)
{
    qDebug() << "Copying text to clipboard";
#if _ZPD_
    qDebug() << "Utils::copyText() - Checking ZPD intercept";
    int intercept = 0;
    if (getLoadLibsInstance()->m_document_clip_copy) {
        qInfo() << "当前文档: *** "/* <<m_currenFilePath*/;
        getLoadLibsInstance()->m_document_clip_copy(m_currenFilePath.toLocal8Bit().data(), &intercept);
        qInfo() << "是否拦截不允许复制(1:拦截 0:不拦截): " << intercept;
    }
    if (intercept > 0) {
        qDebug() << "Utils::copyText() - Copy intercepted by ZPD, returning";
        return;
    }
#endif
    QClipboard *clipboard = DApplication::clipboard();
    QString sOldText = clipboard->text(QClipboard::Clipboard);
    if (sOldText != sText) {
        qDebug() << "Utils::copyText() - Setting new text to clipboard";
        clipboard->setText(sText);
    }
    qDebug() << "Utils::copyText() - Text copy operation completed";
}

QString Utils::getElidedText(const QFontMetrics &fontMetrics, const QSize &size, const QString &text, Qt::Alignment alignment)
{
    qDebug() << "Utils::getElidedText() - Starting text eliding";
    int textLength = text.length();
    QString tmptext;
    for (int index = 0; index < textLength; index++) {
        const QString &ltext = text.left(index + 1);
        if (fontMetrics.boundingRect(0, 0, size.width(), size.height(), static_cast<int>(alignment | Qt::TextWrapAnywhere), ltext + QString("...")).height() > size.height()) {
            qDebug() << "Utils::getElidedText() - Text exceeds height at index:" << index;
            break;
        }
        tmptext = ltext;
    }
    if (tmptext.length() < textLength) {
        qDebug() << "Utils::getElidedText() - Adding ellipsis";
        tmptext.append("...");
    }
    qDebug() << "Utils::getElidedText() - Final elided text length:" << tmptext.length();
    return tmptext;
}

bool Utils::copyFile(const QString &sourcePath, const QString &destinationPath)
{
    qDebug() << "Copying file from:" << sourcePath << "to:" << destinationPath;
    if (sourcePath == destinationPath) {
        qDebug() << "Source and destination paths are the same";
        return true;
    }

    QFile sourceFile(sourcePath);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open source file for reading";
        return false;
    }

    QFile destinationFile(destinationPath);
    if (!destinationFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "Utils::copyFile() - Failed to open destination file, closing source and returning false";
        sourceFile.close();
        return false;
    }

    const qint64 maxSize = 4096;
    qint64 size = -1;

    QScopedArrayPointer< char > buffer(new char[maxSize]);

    int ret = true;
    do {
        if ((size = sourceFile.read(buffer.data(), maxSize)) < 0) {
            qDebug() << "Utils::copyFile() - Error reading from source file";
            ret = false;
            break;
        }

        if (destinationFile.write(buffer.data(), size) < 0) {
            qDebug() << "Utils::copyFile() - Error writing to destination file";
            ret = false;
            break;
        }
    } while (size > 0);

    sourceFile.close();

    destinationFile.flush();//函数将用户缓存中的内容写入内核缓冲区
    fsync(destinationFile.handle());//将内核缓冲写入文件(磁盘)
    destinationFile.close();
    qDebug() << "Utils::copyFile() - Destination file closed, copy operation result:" << ret;
    return ret;
}

QImage Utils::copyImage(const QImage &srcimg, int x, int y, int w, int h)
{
    qDebug() << "Copying image region:" << QRect(x, y, w, h);
    int dx = 0;
    int dy = 0;
    if (w <= 0 || h <= 0) {
        qDebug() << "Utils::copyImage() - Invalid dimensions, returning empty image";
        return QImage();
    }

    QImage::Format format = srcimg.format();
    QImage image(w, h, format);
    if (image.isNull()) {
        qDebug() << "Utils::copyImage() - Failed to create destination image";
        return image;
    }

    int srcw = srcimg.width();
    int srch = srcimg.height();
    if (x < 0 || y < 0 || x + w > srcw || y + h > srch) {
        qDebug() << "Utils::copyImage() - Region extends beyond source image, adjusting coordinates";
        // bitBlt will not cover entire image - clear it.
        image.fill(Qt::white);
        if (x < 0) {
            dx = -x;
            x = 0;
        }
        if (y < 0) {
            dy = -y;
            y = 0;
        }
    }

    image.setColorTable(srcimg.colorTable());

    int pixels_to_copy = qMax(w - dx, 0);
    if (x > srcw)
        pixels_to_copy = 0;
    else if (pixels_to_copy > srcw - x)
        pixels_to_copy = srcw - x;
    int lines_to_copy = qMax(h - dy, 0);
    if (y > srch)
        lines_to_copy = 0;
    else if (lines_to_copy > srch - y)
        lines_to_copy = srch - y;

    bool byteAligned = true;
    if (format == QImage::Format_Mono || format == QImage::Format_MonoLSB)
        byteAligned = !(dx & 7) && !(x & 7) && !(pixels_to_copy & 7);

    if (byteAligned) {
        qDebug() << "Utils::copyImage() - Using byte-aligned copy method";
        const uchar *src = srcimg.bits() + ((x * srcimg.depth()) >> 3) + y * srcimg.bytesPerLine();
        uchar *dest = image.bits() + ((dx * srcimg.depth()) >> 3) + dy * image.bytesPerLine();
        const int bytes_to_copy = (pixels_to_copy * srcimg.depth()) >> 3;
        for (int i = 0; i < lines_to_copy; ++i) {
            memcpy(dest, src, static_cast<size_t>(bytes_to_copy));
            src += srcimg.bytesPerLine();
            dest += image.bytesPerLine();
        }
    } else if (format == QImage::Format_Mono) {
        qDebug() << "Utils::copyImage() - Using Mono format copy method";
        const uchar *src = srcimg.bits() + y * srcimg.bytesPerLine();
        uchar *dest = image.bits() + dy * image.bytesPerLine();
        for (int i = 0; i < lines_to_copy; ++i) {
            for (int j = 0; j < pixels_to_copy; ++j) {
                if (src[(x + j) >> 3] & (0x80 >> ((x + j) & 7)))
                    dest[(dx + j) >> 3] |= (0x80 >> ((dx + j) & 7));
                else
                    dest[(dx + j) >> 3] &= ~(0x80 >> ((dx + j) & 7));
            }
            src += srcimg.bytesPerLine();
            dest += image.bytesPerLine();
        }
    } else { // Format_MonoLSB
        qDebug() << "Utils::copyImage() - Using MonoLSB format copy method";
        Q_ASSERT(format == QImage::Format_MonoLSB);
        const uchar *src = srcimg.bits() + y * srcimg.bytesPerLine();
        uchar *dest = image.bits() + dy * image.bytesPerLine();
        for (int i = 0; i < lines_to_copy; ++i) {
            for (int j = 0; j < pixels_to_copy; ++j) {
                if (src[(x + j) >> 3] & (0x1 << ((x + j) & 7)))
                    dest[(dx + j) >> 3] |= (0x1 << ((dx + j) & 7));
                else
                    dest[(dx + j) >> 3] &= ~(0x1 << ((dx + j) & 7));
            }
            src += srcimg.bytesPerLine();
            dest += image.bytesPerLine();
        }
    }

    image.setDotsPerMeterX(srcimg.dotsPerMeterX());
    image.setDotsPerMeterY(srcimg.dotsPerMeterY());
    image.setDevicePixelRatio(srcimg.devicePixelRatio());
    image.setOffset(srcimg.offset());
    qDebug() << "Utils::copyImage() - Image copy operation completed";
    return image;
}

QList<QColor> Utils::getHiglightColorList()
{
    qDebug() << "Utils::getHiglightColorList() - Getting highlight color list";
    QList<QColor> listColor;
    listColor.append(QColor("#FFA503"));
    listColor.append(QColor("#FF1C49"));
    listColor.append(QColor("#9023FC"));
    listColor.append(QColor("#3468FF"));
    listColor.append(QColor("#00C7E1"));
    listColor.append(QColor("#05EA6B"));
    listColor.append(QColor("#FEF144"));
    listColor.append(QColor("#D5D5D1"));
    qDebug() << "Utils::getHiglightColorList() - Returning" << listColor.size() << "colors";
    return listColor;
}

void Utils::setHiglightColorIndex(int index)
{
    // qDebug() << "Utils::setHiglightColorIndex() - Setting color index to:" << index;
    if (m_colorIndex < 0 || m_colorIndex >= getHiglightColorList().size()) {
        // qDebug() << "Utils::setHiglightColorIndex() - Invalid index, returning";
        return;
    }
    m_colorIndex = index;
    // qDebug() << "Utils::setHiglightColorIndex() - Color index set successfully";
}

QColor Utils::getCurHiglightColor()
{
    // qDebug() << "Utils::getCurHiglightColor() - Getting current highlight color";
    const QList<QColor> &colorlst =  getHiglightColorList();
    int index = qBound(0, m_colorIndex, colorlst.size() - 1);
    // qDebug() << "Utils::getCurHiglightColor() - Using color index:" << index;
    return colorlst.at(index);
}

void Utils::setObjectNoFocusPolicy(QObject *obj)
{
    // qDebug() << "Utils::setObjectNoFocusPolicy() - Setting no focus policy for object children";
    QList<QWidget *> list = obj->findChildren<QWidget *>();
    // qDebug() << "Utils::setObjectNoFocusPolicy() - Found" << list.size() << "child widgets";
    foreach (QWidget *w, list) {
        if (w->objectName().isEmpty()) {
            // qDebug() << "Utils::setObjectNoFocusPolicy() - Setting no focus for unnamed widget";
            w->setFocusPolicy(Qt::NoFocus);
        }
    }
    // qDebug() << "Utils::setObjectNoFocusPolicy() - Focus policy setting completed";
}

bool Utils::isWayland()
{
    qDebug() << "Checking for Wayland environment";
    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));

    if (XDG_SESSION_TYPE == QLatin1String("wayland") || WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive)) {
        qDebug() << "Utils::isWayland() - Wayland environment detected";
        return true;
    } else {
        qDebug() << "Utils::isWayland() - Not a Wayland environment";
        return false;
    }
}

void Utils::setCurrentFilePath(QString currentFilePath)
{
    qDebug() << "Setting current file path:" << currentFilePath;
    m_currenFilePath = currentFilePath;
}

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

// gesture 触控板手势
#define GESTURE_SERVICE          "com.deepin.daemon.Gesture"
#define GESTURE_PATH             "/com/deepin/daemon/Gesture"
#define GESTURE_INTERFACE        "com.deepin.daemon.Gesture"
#define GESTURE_SIGNAL           "Event"

class Utils : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief getKeyshortcut
     * 组合快捷键
     * @param keyEvent
     * @return
     */
    static QString getKeyshortcut(QKeyEvent *keyEvent);

    static bool fileExists(const QString &path);

    static QImage dropShadow(const QPixmap &px, qreal radius, const QColor &color);

    /**
     * @brief getInputDataSize
     * 数据单位换算,最大是G
     * @return
     */
    static QString getInputDataSize(const qint64 &);

    /**
     * @brief roundQPixmap
     * 生成带圆角缩略图
     * @param img_in
     * @param radius
     * @return
     */
    static QPixmap roundQPixmap(const QPixmap &img_in, int radius);

    /**
     * @brief copyText
     *复制文本到系统剪切板
     * @param sText
     */
    static void copyText(const QString &sText);

    /**
     * @brief getUuid
     * 获取UUID(唯一)
     * @return
     */
    static QString getUuid();

    static QString getElidedText(const QFontMetrics &fontMetrics, const QSize &size, const QString &text, Qt::Alignment alignment);

    /**
     * @brief copyFile
     * 复制文件
     * @param sourcePath
     * @param destinationPath
     * @return
     */
    static bool copyFile(const QString &sourcePath, const QString &destinationPath);

    /**
     * @brief copyImage
     * 复制缩略图
     * @param srcimg
     * @param x
     * @param y
     * @param w
     * @param h
     * @return
     */
    static QImage copyImage(const QImage &srcimg, int x, int y, int w, int h);

    /**
     * @brief renderSVG
     * 渲染缩略图
     * @param filePath
     * @param size
     * @return
     */
    static QPixmap renderSVG(const QString &filePath, const QSize &size);

    /**
     * @brief getHiglightColorList
     * 注释高亮颜色列表
     * @return
     */
    static QList<QColor> getHiglightColorList();

    /**
     * @brief getCurHiglightColor
     * 当前注释高亮颜色
     * @return
     */
    static QColor getCurHiglightColor();

    /**
     * @brief setHiglightColorIndex
     * 设置全局注释高亮颜色
     * @param index
     */
    static void setHiglightColorIndex(int index);

    /**
     * @brief setObjectNoFocusPolicy
     * 设置对象的焦点策略
     * @param obj
     */
    static void setObjectNoFocusPolicy(QObject *obj);

    /**
     * @brief isWayland
     * 是否wayLand
     * @return
     */
    static bool isWayland();

private:
    static int m_colorIndex;
};

#endif // UTILS_H

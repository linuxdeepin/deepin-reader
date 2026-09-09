// Copyright (C) 2019 ~ 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BookMarkDelegate.h"
#include "SideBarImageViewModel.h"
#include "Application.h"
#include "ddlog.h"

#include <DGuiApplicationHelper>

#include <QPainter>
#include <QItemSelectionModel>
#include <QAbstractItemView>
#include <QPainterPath>
#include <QImage>

BookMarkDelegate::BookMarkDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
    qCInfo(appLog) << "Creating BookMarkDelegate with parent widget:" << parent;

    m_parent = parent;
    m_darkPixmapCache.setMaxCost(8 * 1024 * 1024); // 反色缓存预算 8MB，按像素字节数计费
}

void BookMarkDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    qCDebug(appLog) << "Painting bookmark item at row:" << index.row();

    if (index.isValid()) {
        qCDebug(appLog) << "Index is valid, proceeding with bookmark item rendering";
        const QPixmap &pixmap = index.data(ImageinfoType_e::IMAGE_PIXMAP).value<QPixmap>();
        QSize pageSize = index.data(ImageinfoType_e::IMAGE_PAGE_SIZE).toSize();

        const int borderRadius = 6;
        pageSize.scale(static_cast<int>(62 * dApp->devicePixelRatio()), static_cast<int>(62 * dApp->devicePixelRatio()), Qt::KeepAspectRatio);
        const QSize &scalePixSize = pageSize / dApp->devicePixelRatio();
        const QRect &rect = QRect(option.rect.x() + 10, option.rect.center().y() - scalePixSize.height() / 2, scalePixSize.width(), scalePixSize.height());

        if (!pixmap.isNull()) {
            qCDebug(appLog) << "Rendering bookmark thumbnail image";
            const QPixmap &scalePix = pixmap.scaled(pageSize);
            //clipPath pixmap
            painter->save();
            QPainterPath clipPath;
            clipPath.addRoundedRect(rect, borderRadius, borderRadius);
            painter->setClipPath(clipPath);
            // 深色主题下将白底缩略图反色为黑底白字（仅绘制时反色，不改缓存原图）：
            // HSL 亮度反转，下限钳制 37(#252525)，反转后 ≥192 提亮纯白。
            if (DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->themeType() == DTK_NAMESPACE::Gui::DGuiApplicationHelper::DarkType) {
                // 按源图 cacheKey() 缓存反色结果，避免每次重绘重复逐像素计算
                QPixmap invertedPixmap;
                if (QPixmap *cached = m_darkPixmapCache.object(pixmap.cacheKey())) {
                    invertedPixmap = *cached;
                } else {
                    QImage img = scalePix.toImage();
                    if (!img.isNull()) {
                        if (img.format() != QImage::Format_ARGB32)
                            img = img.convertToFormat(QImage::Format_ARGB32);
                        const int w = img.width();
                        const int h = img.height();
                        const int kMinLightAfterInvert = 37;       // #252525
                        const int kMaxLightBoostThreshold = 192;   // 0xC0，提亮阈值
                        for (int y = 0; y < h; ++y) {
                            QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
                            for (int x = 0; x < w; ++x) {
                                const QRgb px = line[x];
                                const int alpha = qAlpha(px);
                                QColor c = QColor::fromRgb(qRed(px), qGreen(px), qBlue(px));
                                int hue, sat, light, dummy;
                                c.getHsl(&hue, &sat, &light, &dummy);
                                light = 255 - light;
                                if (light >= kMaxLightBoostThreshold)
                                    light = 255;
                                light = qMax(light, kMinLightAfterInvert);
                                c.setHsl(hue, sat, light);
                                line[x] = qRgba(c.red(), c.green(), c.blue(), alpha);
                            }
                        }
                        invertedPixmap = QPixmap::fromImage(img);
                        invertedPixmap.setDevicePixelRatio(scalePix.devicePixelRatio());
                        m_darkPixmapCache.insert(pixmap.cacheKey(), new QPixmap(invertedPixmap),
                                                 img.width() * img.height() * 4); // ARGB32 每像素固定 4 字节
                    }
                }
                if (!invertedPixmap.isNull())
                    painter->drawPixmap(rect.x(), rect.y(), invertedPixmap);
                else
                    painter->drawPixmap(rect.x(), rect.y(), scalePix);
            } else {
                painter->drawPixmap(rect.x(), rect.y(), scalePix);
            }
            painter->restore();
        }

        //drawText RoundRect
        painter->save();
        painter->setBrush(Qt::NoBrush);
        painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        bool isSelected = m_parent->selectionModel()->isRowSelected(index.row(), index.parent());
        qCDebug(appLog) << "Drawing selection border - selected:" << isSelected;
        if (isSelected) {
            painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().highlight().color(), 2));
            painter->drawRoundedRect(rect, borderRadius, borderRadius);
        } else {
            // 未选中：深色主题下 frameShadowBorder 与深色背景混色，改用 windowText@0.2α
            QColor frameColor = DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().frameShadowBorder().color();
            if (DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->themeType() == DTK_NAMESPACE::Gui::DGuiApplicationHelper::DarkType) {
                frameColor = DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().windowText().color();
                frameColor.setAlphaF(0.2);
            }
            painter->setPen(QPen(frameColor, 1));
            painter->drawRoundedRect(rect, borderRadius, borderRadius);
        }
        painter->restore();

        //drawPagetext
        int margin = 2;
        int bottomlineHeight = 1;
        painter->save();
        painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().windowText().color()));
        QFont font = painter->font();
        font = DFontSizeManager::instance()->t8(font);
        painter->setFont(font);
        const QString &pageText = index.data(ImageinfoType_e::IMAGE_INDEX_TEXT).toString();
        qCDebug(appLog) << "Rendering page number text:" << pageText;
        int pagetextHeight = painter->fontMetrics().height();
        painter->drawText(rect.right() + 18, option.rect.y() + margin, option.rect.width(), pagetextHeight, Qt::AlignVCenter | Qt::AlignLeft, pageText);
        painter->restore();

        //drawBottomLine
        painter->save();
        painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().frameBorder().color(), bottomlineHeight));
        painter->drawLine(rect.right() + 18, option.rect.bottom() - bottomlineHeight, option.rect.right(), option.rect.bottom() - bottomlineHeight);
        painter->restore();
    }
}

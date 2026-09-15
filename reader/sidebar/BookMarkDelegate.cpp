// Copyright (C) 2019 ~ 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BookMarkDelegate.h"
#include "SideBarImageViewModel.h"
#include "NightFilter.h"
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
            // 统一走主干夜间滤镜 NightFilter（CIELAB L* 反转），图片对象区域不反色
            // （照片零负片，与主视图蒙版行为一致），含扫描页整页反色特判。
            if (DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->themeType() == DTK_NAMESPACE::Gui::DGuiApplicationHelper::DarkType) {
                // 图片对象 bbox（与存储缩略图像素对齐，渲染线程预取）
                const QVector<QRectF> imageRects = index.data(ImageinfoType_e::IMAGE_NIGHT_MASK).value<QVector<QRectF>>();
                // 按源缩略图 cacheKey() 缓存反色结果，避免每次重绘重复逐像素计算
                QPixmap invertedPixmap;
                if (QPixmap *cached = m_darkPixmapCache.object(pixmap.cacheKey())) {
                    invertedPixmap = *cached;
                } else {
                    // bbox 是存储缩略图（174px）坐标，而 scalePix 是等比缩小后的图，
                    // 须先映射到 scalePix 坐标，否则覆盖率特判/蒙版区域全错
                    QVector<QRectF> scaledRects;
                    scaledRects.reserve(imageRects.size());
                    const qreal sx = pixmap.isNull() || pixmap.width() == 0
                                             ? 0.0 : qreal(scalePix.width()) / pixmap.width();
                    const qreal sy = pixmap.isNull() || pixmap.height() == 0
                                             ? 0.0 : qreal(scalePix.height()) / pixmap.height();
                    for (const QRectF &r : imageRects)
                        scaledRects.append(QRectF(r.x() * sx, r.y() * sy,
                                                  r.width() * sx, r.height() * sy));

                    invertedPixmap = QPixmap::fromImage(NightFilter::applyPage(scalePix.toImage(), scaledRects));
                    invertedPixmap.setDevicePixelRatio(scalePix.devicePixelRatio());
                    if (!invertedPixmap.isNull())
                        m_darkPixmapCache.insert(pixmap.cacheKey(), new QPixmap(invertedPixmap),
                                                 invertedPixmap.width() * invertedPixmap.height() * 4); // ARGB32 每像素固定 4 字节
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

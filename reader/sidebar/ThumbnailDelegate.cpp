// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ThumbnailDelegate.h"
#include "SideBarImageViewModel.h"
#include "NightFilter.h"
#include "Utils.h"
#include "Application.h"
#include "ddlog.h"

#include <DGuiApplicationHelper>
#include <QDebug>

#include <QPainter>
#include <QItemSelectionModel>
#include <QAbstractItemView>
#include <QPainterPath>
#include <QTransform>
#include <QImage>
ThumbnailDelegate::ThumbnailDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
    // qCDebug(appLog) << "ThumbnailDelegate created with parent:" << parent;
    m_parent = parent;
}


void ThumbnailDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // qCDebug(appLog) << "ThumbnailDelegate::paint start - row:" << index.row();
    if (index.isValid()) {
        // qCDebug(appLog) << "Valid index, proceeding with paint";
        qreal pixscale = m_parent->property("adaptScale").toDouble();

        int rotate = index.data(ImageinfoType_e::IMAGE_ROTATE).toInt();

        bool bShowBookMark = index.data(ImageinfoType_e::IMAGE_BOOKMARK).toBool();

        QTransform transform;

        transform.rotate(rotate);

        const QPixmap &rawPixmap = index.data(ImageinfoType_e::IMAGE_PIXMAP).value<QPixmap>();

        const QPixmap &pixmap = rawPixmap.transformed(transform);

        const int borderRadius = 6;

        QSize pageSize = index.data(ImageinfoType_e::IMAGE_PAGE_SIZE).toSize();

        if (rotate == 90 || rotate == 270)
            pageSize = QSize(pageSize.height(), pageSize.width());

        pageSize.scale(static_cast<int>(174 * pixscale * dApp->devicePixelRatio()), static_cast<int>(174 * pixscale * dApp->devicePixelRatio()), Qt::KeepAspectRatio);

        const QSize &scalePixSize = pageSize / dApp->devicePixelRatio();

        const QRect &rect = QRect(option.rect.center().x() - scalePixSize.width() / 2, option.rect.center().y() - scalePixSize.height() / 2, scalePixSize.width(), scalePixSize.height());

        if (!pixmap.isNull()) {
            // qCDebug(appLog) << "Drawing pixmap at:" << rect;
            //clipPath pixmap
            painter->save();
            QPainterPath clipPath;
            clipPath.addRoundedRect(rect, borderRadius, borderRadius);
            painter->setClipPath(clipPath);
            // 缩略图反色只跟随系统深色主题：深色主题下白底文档反转为黑底白字
            // （与书签/注释列表观感一致），浅色主题照常绘制原图。
            // 反色统一走主干夜间滤镜 NightFilter（CIELAB L* 反转，不再用旧 HSL 方案），
            // 图片对象区域蒙版随缩略图由渲染线程预取（与主视图同源），照片区域不反色，
            // 并含扫描页覆盖率特判（超过阈值整页反色，避免回贴/蒙版异常）
            const bool darkTheme = (DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->themeType() == DTK_NAMESPACE::Gui::DGuiApplicationHelper::DarkType);
            const QVector<QRectF> imageRects = index.data(ImageinfoType_e::IMAGE_NIGHT_MASK).value<QVector<QRectF>>();

            // 反色结果按未旋转的原始缩略图缓存，再叠加旋转，避免每次重绘都逐像素反色
            const QPixmap displayPixmap = darkTheme
                                          ? nightPixmap(rawPixmap, imageRects).transformed(transform)
                                          : pixmap;

            painter->drawPixmap(rect.x(), rect.y(), rect.width(), rect.height(), displayPixmap);
            painter->restore();
        }

        //drawText RoundRect
        painter->save();
        painter->setBrush(Qt::NoBrush);
        painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
        if (m_parent->selectionModel()->isRowSelected(index.row(), index.parent())) {
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
            painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().windowText().color()));
        }
        painter->drawText(rect.x(), rect.bottom() + 4, rect.width(), option.rect.bottom() - rect.bottom(), Qt::AlignHCenter | Qt::AlignTop, QString::number(index.row() + 1));
        painter->restore();
        drawBookMark(painter, rect, bShowBookMark);
    }
    // qCDebug(appLog) << "ThumbnailDelegate::paint end";
}
QSize ThumbnailDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    // qCDebug(appLog) << "Calculating size hint for row:" << index.row();
    return DStyledItemDelegate::sizeHint(option, index);
}

QPixmap ThumbnailDelegate::nightPixmap(const QPixmap &src, const QVector<QRectF> &imageRects) const
{
    if (src.isNull())
        return src;

    // 滚动/选中时同一张缩略图会被反复重绘，缓存反色结果避免逐像素重复计算；
    // 缩略图重渲染时 cacheKey 必然变化，无需将蒙版纳入缓存键
    if (m_nightSourceCache.cacheKey() == src.cacheKey() && !m_nightPixmapCache.isNull())
        return m_nightPixmapCache;

    m_nightSourceCache = src;
    m_nightPixmapCache = QPixmap::fromImage(NightFilter::applyPage(src.toImage(), imageRects));
    m_nightPixmapCache.setDevicePixelRatio(src.devicePixelRatio());
    return m_nightPixmapCache;
}

void ThumbnailDelegate::drawBookMark(QPainter *painter, const QRect &rect, bool visible) const
{
    // qCDebug(appLog) << "Drawing bookmark at:" << rect;
    if (visible) {
        // qCDebug(appLog) << "Drawing bookmark at:" << rect;
        QPixmap pixmap(QIcon::fromTheme("dr_bookmark_checked").pixmap({36, 36}));
        painter->drawPixmap(rect.right() - 42, rect.y(), pixmap);
    } else {
        // qCDebug(appLog) << "Bookmark not visible for rect:" << rect;
    }
}

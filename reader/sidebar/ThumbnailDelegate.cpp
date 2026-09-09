// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ThumbnailDelegate.h"
#include "SideBarImageViewModel.h"
#include "EyeProtectionManager.h"
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
            // 缩略图外观必须与主视图(BrowserPage::paint)保持一致，因此跟随护眼模式而非
            // 系统深浅主题：此前按系统深色主题反色，深色主题 + 无护眼时会出现主视图
            // 仍是白底、侧边栏缩略图却是黑底的不一致（尤其白底的 OFD/PDF 文档）。
            // 使用主干的夜间滤镜；缩略图尚无图片对象蒙版，采用其整页回退路径：
            //   Night           → CIELAB 明度反转 + 深色压暗
            //   Classic/Green   → 正片叠底(Multiply)染色
            //   Off             → 原图
            EyeProtectionManager *epMgr = EyeProtectionManager::instance();
            const EyeProtectionManager::Mode mode = epMgr->mode();

            // 反色结果按未旋转的原始缩略图缓存，再叠加旋转，避免每次重绘都逐像素反色
            const QPixmap displayPixmap = (mode == EyeProtectionManager::Night)
                                          ? nightPixmap(rawPixmap).transformed(transform)
                                          : pixmap;

            painter->drawPixmap(rect.x(), rect.y(), rect.width(), rect.height(), displayPixmap);

            if (mode == EyeProtectionManager::Night) {
                // 与主视图一致：叠加轻微深色半透明层降低整体亮度
                QColor dark = epMgr->pageBackgroundColor();
                dark.setAlpha(60);
                painter->fillRect(rect, dark);
            } else if (mode != EyeProtectionManager::Off) {
                // 与主视图一致：经典/绿色护眼用正片叠底染色
                painter->setCompositionMode(QPainter::CompositionMode_Multiply);
                painter->fillRect(rect, epMgr->pageBackgroundColor());
                painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
            }
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

QPixmap ThumbnailDelegate::nightPixmap(const QPixmap &src) const
{
    if (src.isNull())
        return src;

    // 滚动/选中时同一张缩略图会被反复重绘，缓存反色结果避免逐像素重复计算
    if (m_nightSourceCache.cacheKey() == src.cacheKey() && !m_nightPixmapCache.isNull())
        return m_nightPixmapCache;

    m_nightSourceCache = src;
    m_nightPixmapCache = QPixmap::fromImage(NightFilter::applyPage(src.toImage(), {}));
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

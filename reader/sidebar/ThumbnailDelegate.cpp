// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ThumbnailDelegate.h"
#include "SideBarImageViewModel.h"
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

        const QPixmap &pixmap = index.data(ImageinfoType_e::IMAGE_PIXMAP).value<QPixmap>().transformed(transform);

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
            // 深色系统主题下，缩略图卡片需与侧边栏深色背景协调：将文档原始白底黑字
            // 的缩略图反色为黑底白字；浅色主题保持原样。反色仅在绘制时进行，
            // 不修改 DocSheet 中缓存的真实缩略图(始终为白底)，避免主题切换时双重反色。
            // 采用与 BrowserPage::applyNightMode 相同的 HSL 亮度反转算法：
            // 仅反转 Lightness 通道，保留 Hue/Saturation，避免图片色相偏移 180°。
            //   白底黑字 → 黑底白字（文字/背景正确反色）
            //   彩色图片/链接 → 仅变暗，色相保持
            if (DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->themeType() == DTK_NAMESPACE::Gui::DGuiApplicationHelper::DarkType) {
                QImage img = pixmap.toImage();
                if (!img.isNull()) {
                    if (img.format() != QImage::Format_ARGB32)
                        img = img.convertToFormat(QImage::Format_ARGB32);
                    const int w = img.width();
                    const int h = img.height();
                    for (int y = 0; y < h; ++y) {
                        QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
                        for (int x = 0; x < w; ++x) {
                            const QRgb px = line[x];
                            const int alpha = qAlpha(px);
                            QColor c = QColor::fromRgb(qRed(px), qGreen(px), qBlue(px));
                            int hue, sat, light, dummy;
                            c.getHsl(&hue, &sat, &light, &dummy);
                            light = 255 - light;
                            c.setHsl(hue, sat, light);
                            line[x] = qRgba(c.red(), c.green(), c.blue(), alpha);
                        }
                    }
                    QPixmap invertedPixmap = QPixmap::fromImage(img);
                    invertedPixmap.setDevicePixelRatio(pixmap.devicePixelRatio());
                    painter->drawPixmap(rect.x(), rect.y(), rect.width(), rect.height(), invertedPixmap);
                } else {
                    painter->drawPixmap(rect.x(), rect.y(), rect.width(), rect.height(), pixmap);
                }
            } else {
                painter->drawPixmap(rect.x(), rect.y(), rect.width(), rect.height(), pixmap);
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
            painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().frameShadowBorder().color(), 1));
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

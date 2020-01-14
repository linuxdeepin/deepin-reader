/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#include "CatalogDelegate.h"

#include <DApplicationHelper>
#include <QPainter>
#include <DPalette>
#include <QDebug>

CatalogDelegate::CatalogDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
    this->parent = parent;
}

void CatalogDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (!index.isValid()) {
        DStyledItemDelegate::paint(painter, option, index);
        return;
    }

    painter->save();
    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter->setOpacity(1);

    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    DPalette::ColorGroup cg;

    cg = DPalette::Active;

    int radius = 8;
    int margin = 8;

    DApplicationHelper *dAppHelper = DApplicationHelper::instance();
    DPalette palette = dAppHelper->applicationPalette();
    QBrush background;
    QPen forground;
    if (opt.features & QStyleOptionViewItem::Alternate) {
        background = palette.color(cg, DPalette::AlternateBase);
    } else {
        background = palette.color(cg, DPalette::Base);
    }

    forground.setColor(palette.color(cg, DPalette::Text));
    if (opt.state & QStyle::State_Selected) {
        background = palette.color(cg, DPalette::Highlight);
        forground.setColor(palette.color(cg, DPalette::HighlightedText));
    }

    painter->setPen(forground);

    QRect rect = opt.rect;
    QFontMetrics fm(opt.font);
    QPainterPath path, clipPath;
    QRect textRect = rect;

    switch (opt.viewItemPosition) {
    case DStyleOptionViewItem::Beginning: {
        rect.setX(rect.x() + margin);  // left margin

        QPainterPath rectPath, roundedPath;
        roundedPath.addRoundedRect(rect.x(), rect.y(), rect.width() * 2, rect.height(), radius, radius);
        rectPath.addRect(rect.x() + rect.width(), rect.y(), rect.width(), rect.height());
        clipPath = roundedPath.subtracted(rectPath);
        painter->setClipPath(clipPath);
        path.addRect(rect);
    } break;
    case DStyleOptionViewItem::Middle: {
        path.addRect(rect);
    } break;
    case DStyleOptionViewItem::End: {
        rect.setWidth(rect.width() - margin);  // right margin

        QPainterPath rectPath, roundedPath;
        roundedPath.addRoundedRect(rect.x() - rect.width(), rect.y(), rect.width() * 2, rect.height(), radius, radius);
        rectPath.addRect(rect.x() - rect.width(), rect.y(), rect.width(), rect.height());
        clipPath = roundedPath.subtracted(rectPath);
        painter->setClipPath(clipPath);
        path.addRect(rect);
    } break;
    case DStyleOptionViewItem::OnlyOne: {
        rect.setX(rect.x() + margin);          // left margin
        rect.setWidth(rect.width() - margin);  // right margin
        path.addRoundedRect(rect, radius, radius);
    } break;
    default: {
        painter->restore();
        DStyledItemDelegate::paint(painter, option, index);
        return;
    }
    }
    painter->fillPath(path, background);

    textRect = rect;
    textRect.setX(textRect.x() + margin - 2);
    QString text = fm.elidedText(opt.text, opt.textElideMode, textRect.width());

    if (index.column() == 0) {
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter, text);
    } else if (index.column() == 2) {
        painter->drawText(textRect, Qt::AlignRight | Qt::AlignVCenter, text);
    }

    painter->restore();
}

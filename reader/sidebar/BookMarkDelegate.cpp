// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
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

BookMarkDelegate::BookMarkDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
    qCInfo(appLog) << "Creating BookMarkDelegate with parent widget:" << parent;

    m_parent = parent;
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
            painter->drawPixmap(rect.x(), rect.y(), scalePix);
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
            painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().frameShadowBorder().color(), 1));
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

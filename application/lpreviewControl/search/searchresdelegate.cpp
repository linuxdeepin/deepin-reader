/*
* Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
*
* Author:     leiyu <leiyu@live.com>
* Maintainer: leiyu <leiyu@deepin.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "searchresdelegate.h"
#include "lpreviewControl/ImageViewModel.h"

#include <QPainter>
#include <QDebug>
#include <QItemSelectionModel>
#include <QAbstractItemView>

#include <DGuiApplicationHelper>

SearchResDelegate::SearchResDelegate(QAbstractItemView *parent)
    : DStyledItemDelegate(parent)
{
    m_parent = parent;
}

void SearchResDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    if (index.isValid()) {
        const QPixmap &pixmap = index.data(ImageinfoType_e::IMAGE_PIXMAP).value<QPixmap>();
        if (!pixmap.isNull()) {
            const int borderRadius = 6;
            const QPixmap &scalePix = pixmap.scaled(62, 62, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            const QSize &scalePixSize = scalePix.size();
            const QRect &rect = QRect(option.rect.x() + 10, option.rect.center().y() - scalePixSize.height() / 2, scalePixSize.width(), scalePixSize.height());

            //clipPath pixmap
            painter->save();
            QPainterPath clipPath;
            clipPath.addRoundedRect(rect, borderRadius, borderRadius);
            painter->setClipPath(clipPath);
            painter->drawPixmap(rect.x(), rect.y(), scalePix);
            painter->restore();
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
            }
            painter->restore();

            //drawPagetext
            int textStartX = rect.right() + 18;
            painter->save();
            painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().windowText().color()));
            QFont font = painter->font();
            font = DFontSizeManager::instance()->t8(font);
            painter->setFont(font);
            const QString &pageText = index.data(ImageinfoType_e::IMAGE_INDEX_TEXT).toString();
            int pagetextHeight = painter->fontMetrics().height();
            painter->drawText(textStartX, rect.y(), option.rect.width(), pagetextHeight, Qt::AlignVCenter | Qt::AlignLeft, pageText);
            painter->restore();

            //drawSearchCount
            painter->save();
            painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().textTips().color()));
            QFont sfont = painter->font();
            sfont = DFontSizeManager::instance()->t10(sfont);
            painter->setFont(sfont);
            const QString &searchCountText = index.data(ImageinfoType_e::IMAGE_SEARCH_COUNT).toString();
            int searchCountTextWidth = painter->fontMetrics().width(searchCountText);
            painter->drawText(option.rect.right() - searchCountTextWidth - 20, rect.y(), option.rect.width(), pagetextHeight, Qt::AlignVCenter | Qt::AlignLeft, searchCountText);
            painter->restore();

            //drawPageContenttext
            painter->save();
            painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().brightText().color()));
            QFont cfont = painter->font();
            cfont = DFontSizeManager::instance()->t9(cfont);
            painter->setFont(cfont);
            QString contentText = index.data(ImageinfoType_e::IMAGE_CONTENT_TEXT).toString();
            contentText.replace(QChar('\n'), QString(""));
            contentText.replace(QChar('\t'), QString(""));
            QTextOption contentOption;
            contentOption.setAlignment(Qt::AlignTop | Qt::AlignLeft);
            contentOption.setWrapMode(QTextOption::WrapAnywhere);

            QSize contentSize(option.rect.right() - textStartX, option.rect.bottom() - pagetextHeight - rect.y() - 10);
            int contentHeight = painter->fontMetrics().height();
            int maxLineCount = qMax(contentSize.height() / contentHeight, 1);
            int maxContentWidth = contentSize.width() * maxLineCount;
            const QString &elidedContentText = painter->fontMetrics().elidedText(contentText, Qt::ElideRight, maxContentWidth);
            painter->drawText(QRect(textStartX, rect.y() + pagetextHeight, contentSize.width(), contentSize.height()), elidedContentText, contentOption);
            painter->restore();

            //drawBottomLine
            painter->save();
            int bottomlineHeight = 1;
            painter->setPen(QPen(DTK_NAMESPACE::Gui::DGuiApplicationHelper::instance()->applicationPalette().frameBorder().color(), bottomlineHeight));
            painter->drawLine(textStartX, option.rect.bottom() - bottomlineHeight, option.rect.right(), option.rect.bottom() - bottomlineHeight);
            painter->restore();
        }
    }
}

QSize SearchResDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return DStyledItemDelegate::sizeHint(option, index);
}


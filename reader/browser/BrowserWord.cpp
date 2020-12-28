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
#include "BrowserWord.h"
#include "BrowserPage.h"
#include "Model.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

BrowserWord::BrowserWord(QGraphicsItem *parent, deepin_reader::Word word) : QGraphicsItem(parent), m_word(word)
{
    setZValue(deepin_reader::Z_ORDER_SELECT_TEXT);
}

void BrowserWord::setScaleFactor(qreal scaleFactor)
{
    m_scaleFactor = scaleFactor;
    update();
}

QString BrowserWord::text()
{
    return m_word.text;
}

void BrowserWord::setSelectable(bool enable)
{
    m_selectable = enable;

    setFlag(QGraphicsItem::ItemIsSelectable, enable);
}

QRectF BrowserWord::boundingRect() const
{
    return QRectF(m_word.boundingBox.x() * m_scaleFactor - 1, m_word.boundingBox.y() * m_scaleFactor - 1, m_word.boundingBox.width() * m_scaleFactor + 2, m_word.boundingBox.height() * m_scaleFactor + 2);
}

QRectF BrowserWord::textBoundingRect() const
{
    return QRectF(m_word.boundingBox.x(), m_word.boundingBox.y(), m_word.boundingBox.width(), m_word.boundingBox.height());
}

QRectF BrowserWord::boundingBox() const
{
    return m_word.boundingBox;
}

void BrowserWord::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if (isSelected()) {
        painter->setBrush(QColor(72, 118, 255, 100));
        painter->setPen(Qt::NoPen);
        painter->drawRect(option->rect.x() + 1, option->rect.y() + 1, option->rect.width() - 2, option->rect.height() - 2);
    }
}

void BrowserWord::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    if (m_selectable)
        return;     //在可以选中的情况下不允许单选

    QGraphicsItem::mousePressEvent(e);
}

void BrowserWord::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    if (m_selectable)
        return;     //在可以选中的情况下不允许单选

    QGraphicsItem::mouseReleaseEvent(e);
}

Word BrowserWord::getWord()
{
    return m_word;
}

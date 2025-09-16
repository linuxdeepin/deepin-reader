// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BrowserWord.h"
#include "BrowserPage.h"
#include "Model.h"
#include "ddlog.h"

#include <QPainter>
#include <QDebug>
#include <QStyleOptionGraphicsItem>

BrowserWord::BrowserWord(QGraphicsItem *parent, deepin_reader::Word word) : QGraphicsItem(parent), m_word(word)
{
    // qCDebug(appLog) << "BrowserWord created, text:" << word.text;
    setZValue(deepin_reader::Z_ORDER_SELECT_TEXT);
    // qCDebug(appLog) << "BrowserWord::BrowserWord() - Constructor completed";
}

void BrowserWord::setScaleFactor(qreal scaleFactor)
{
    // qCDebug(appLog) << "BrowserWord::setScaleFactor:" << scaleFactor;
    m_scaleFactor = scaleFactor;
    update();
    // qCDebug(appLog) << "BrowserWord::setScaleFactor() - Set scale factor completed";
}

QString BrowserWord::text()
{
    // qCDebug(appLog) << "BrowserWord::text() - Returning word text:" << m_word.text;
    return m_word.text;
}

void BrowserWord::setSelectable(bool enable)
{
    // qCDebug(appLog) << "BrowserWord::setSelectable:" << enable;
    m_selectable = enable;

    setFlag(QGraphicsItem::ItemIsSelectable, enable);
    // qCDebug(appLog) << "BrowserWord::setSelectable() - Set selectable completed";
}

QRectF BrowserWord::boundingRect() const
{
    // qCDebug(appLog) << "BrowserWord::boundingRect() - Calculating bounding rectangle";
    QRectF rect = QRectF(m_word.boundingBox.x() * m_scaleFactor - 1, m_word.boundingBox.y() * m_scaleFactor - 1, m_word.boundingBox.width() * m_scaleFactor + 2, m_word.boundingBox.height() * m_scaleFactor + 2);
    // qCDebug(appLog) << "BrowserWord::boundingRect() - Bounding rectangle:" << rect;
    return rect;
}

QRectF BrowserWord::boundingBox() const
{
    // qCDebug(appLog) << "BrowserWord::boundingBox() - Returning word bounding box:" << m_word.boundingBox;
    return m_word.boundingBox;
}

void BrowserWord::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    // qCDebug(appLog) << "BrowserWord::paint() - Starting paint operation";
    if (isSelected()) {
        // qCDebug(appLog) << "BrowserWord::paint - selected:" << m_word.text;
        painter->setBrush(QColor(72, 118, 255, 100));
        painter->setPen(Qt::NoPen);
        painter->drawRect(option->rect.x() + 1, option->rect.y() + 1, option->rect.width() - 2, option->rect.height() - 2);
    }
    // qCDebug(appLog) << "BrowserWord::paint() - Paint operation completed";
}

void BrowserWord::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
    // qCDebug(appLog) << "BrowserWord::mousePressEvent() - Starting mouse press event";
    if (m_selectable) {
        // qCDebug(appLog) << "BrowserWord::mousePressEvent() - Word is selectable, ignoring single click";
        return;     //在可以选中的情况下不允许单选
    }

    QGraphicsItem::mousePressEvent(e);
    // qCDebug(appLog) << "BrowserWord::mousePressEvent() - Mouse press event completed";
}

void BrowserWord::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
    // qCDebug(appLog) << "BrowserWord::mouseReleaseEvent() - Starting mouse release event";
    if (m_selectable) {
        // qCDebug(appLog) << "BrowserWord::mouseReleaseEvent() - Word is selectable, ignoring single click";
        return;     //在可以选中的情况下不允许单选
    }

    QGraphicsItem::mouseReleaseEvent(e);
    // qCDebug(appLog) << "BrowserWord::mouseReleaseEvent() - Mouse release event completed";
}

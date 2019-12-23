/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "RoundColorWidget.h"

#include <DStyle>

#include <QColor>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

DWIDGET_USE_NAMESPACE

RoundColorWidget::RoundColorWidget(const QColor &color, QWidget *parent)
    : DWidget(parent)
    , m_isSelected(false)
    , m_color(color)
{
}

bool RoundColorWidget::isSelected()
{
    return m_isSelected;
}

void RoundColorWidget::setSelected(bool selected)
{
    if (m_isSelected == selected)
        return;

    m_isSelected = selected;

    update();
}

void RoundColorWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_isSelected) return;
        Q_EMIT clicked();
    }
}

void RoundColorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);

    QRect squareRect = rect();
    if (m_isSelected) {
        //draw select circle
        QPen pen;
        pen.setBrush(QBrush(m_color));
        pen.setWidth(borderWidth);  //pen width
        painter.setPen(pen);
        QRect r = squareRect.adjusted(3, 3, -3, -3);
        painter.drawEllipse(r);
    }

    QPainterPath path;
    QRect r = squareRect.adjusted(5, 5, -5, -5);
    path.addEllipse(r);
    painter.setClipPath(path);
    painter.drawPath(path);
    painter.fillPath(path, QBrush(m_color));
}

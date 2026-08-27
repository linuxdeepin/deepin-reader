// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "RoundColorWidget.h"
#include "ddlog.h"
#include <QDebug>

#include <DStyle>
#include <DGuiApplicationHelper>

#include <QColor>
#include <QPainter>
#include <QApplication>
#include <QPainterPath>

DWIDGET_USE_NAMESPACE
using namespace Dtk::Gui;

RoundColorWidget::RoundColorWidget(const QColor &color, QWidget *parent)
    : DWidget(parent)
    , m_isSelected(false)
    , m_color(color)
{
    qCDebug(appLog) << "RoundColorWidget created, color:" << color.name() << ", parent:" << parent;
}

void RoundColorWidget::setSelected(bool selected)
{
    qCDebug(appLog) << "RoundColorWidget selection changed from" << m_isSelected << "to" << selected;
    if (m_isSelected == selected)
        return;

    m_isSelected = selected;

    update();
}

void RoundColorWidget::mousePressEvent(QMouseEvent *event)
{
    // qCDebug(appLog) << "RoundColorWidget mousePressEvent";
    if (event->button() == Qt::LeftButton) {
        // qCDebug(appLog) << "RoundColorWidget clicked, selected:" << m_isSelected << ", allNotify:" << m_allnotify;
        if (m_isSelected && !m_allnotify) return;
        Q_EMIT clicked();
    }
}

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
void RoundColorWidget::enterEvent(QEvent *event)
#else
void RoundColorWidget::enterEvent(QEnterEvent *event)
#endif
{
    qCDebug(appLog) << "RoundColorWidget enterEvent";
    m_isHovered = true;
    update();
    DWidget::enterEvent(event);
}

void RoundColorWidget::leaveEvent(QEvent *event)
{
    qCDebug(appLog) << "RoundColorWidget leaveEvent";
    m_isHovered = false;
    update();
    DWidget::leaveEvent(event);
}

void RoundColorWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setRenderHints(QPainter::Antialiasing);

    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);

    QRect squareRect = rect();
    if (m_isSelected) {
        QColor highlightColor = DGuiApplicationHelper::instance()->applicationPalette().highlight().color();
        QPen pen;
        pen.setBrush(QBrush(highlightColor));
        pen.setWidth(borderWidth);  //pen width
        painter.setPen(pen);
        QRect r = squareRect.adjusted(3, 3, -3, -3);
        painter.drawEllipse(r);
    } else if (m_isHovered) {
        bool isDark = DGuiApplicationHelper::instance()->themeType() == DGuiApplicationHelper::DarkType;
        QColor hoverColor = isDark ? Qt::white : Qt::black;
        hoverColor.setAlpha(38);
        QPen pen;
        pen.setBrush(QBrush(hoverColor));
        pen.setWidth(borderWidth);
        painter.setPen(pen);
        QRect r = squareRect.adjusted(3, 3, -3, -3);
        painter.drawEllipse(r);
    }

    QPainterPath path;
    QRect r = squareRect.adjusted(5, 5, -5, -5);
    path.addEllipse(r);
    painter.setClipPath(path);
    painter.fillPath(path, QBrush(m_color));

    if (!m_isSelected) {
        painter.setClipping(false);
        QColor borderColor = palette().color(QPalette::WindowText);
        borderColor.setAlpha(60);
        painter.setPen(QPen(borderColor, 1));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(r);
    }
}

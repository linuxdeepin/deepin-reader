// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "WordWrapLabel.h"
#include <QDebug>

#include <QPainter>

WordWrapLabel::WordWrapLabel(QWidget *parent)
    : DLabel(parent)
{
    qDebug() << "WordWrapLabel created, parent:" << parent;
    m_margin = 0;
}

void WordWrapLabel::setText(const QString &text)
{
    qDebug() << "Setting label text, length:" << text.length();
    m_text = text;
    update();
}

void WordWrapLabel::setMargin(int margin)
{
    qDebug() << "Setting label margin:" << margin;
    m_margin = margin;
    update();
}

void WordWrapLabel::paintEvent(QPaintEvent *event)
{
    DLabel::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    QTextOption option;
    adjustContent();
    option.setAlignment(this->alignment());
    option.setWrapMode(QTextOption::WrapAnywhere);
    painter.setFont(this->font());
    painter.drawText(QRect(m_margin, 0, this->width() -  2 * m_margin, this->height()), m_text, option);
}

void WordWrapLabel::adjustContent()
{
    qDebug() << "Adjusting label content height";
    QFontMetrics fontMetris(this->font());
    int wordHeight = fontMetris.boundingRect(0, 0, this->width() - 2 * m_margin, 0, static_cast<int>(this->alignment() | Qt::TextWrapAnywhere), m_text).height();
    if (this->height() == wordHeight) return;
    setFixedSize(this->width(), wordHeight);
}

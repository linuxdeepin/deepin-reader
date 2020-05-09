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
#include "wordwraplabel.h"

#include <QPainter>
#include <QDebug>

WordWrapLabel::WordWrapLabel(QWidget* parent)
    : DLabel(parent)
{
    m_margin = 0;
}

void WordWrapLabel::setText(const QString& text)
{
    m_text = text;
    update();
}

void WordWrapLabel::setMargin(int margin)
{
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
    QFontMetrics fontMetris(this->font());
    int wordHeight = fontMetris.boundingRect(0, 0, this->width() - 2 * m_margin, 0, this->alignment() | Qt::TextWrapAnywhere, m_text).height();
    if(this->height() == wordHeight) return;
    setFixedSize(this->width(), wordHeight);
}

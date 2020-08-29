/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan <wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan <wangzhixuan@uniontech.com>
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
#include "TransparentTextEdit.h"
#include "Application.h"
#include "MsgHeader.h"

#include <DFontSizeManager>

#include <QPainter>
#include <QDebug>
#include <QScrollBar>
#include <QTextBlock>
#include <QMimeData>

TransparentTextEdit::TransparentTextEdit(DWidget *parent)
    : QTextEdit(parent),
      m_showMenuTimer(nullptr)
{
//    grabGesture(Qt::TapGesture);
//    grabGesture(Qt::TapAndHoldGesture);
//    grabGesture(Qt::SwipeGesture);
//    grabGesture(Qt::PanGesture);
//    grabGesture(Qt::PinchGesture);

    this->setObjectName("TransparentTextEdit");

    init();

//    m_showMenuTimer = new QTimer(this);
//    m_showMenuTimer->setInterval(1000);
//    connect(m_showMenuTimer, &QTimer::timeout, this, [ = ] {
//        m_showMenuTimer->stop();
//        qInfo()  << "         show   text  edit  menu ...  ";
//        QMouseEvent showMenuEvent(QEvent::MouseButtonPress, QCursor::pos(), Qt::NoButton, Qt::RightButton, Qt::NoModifier);
//        QCoreApplication::sendEvent(this, &showMenuEvent);
//        this->show();
//    });

}

void TransparentTextEdit::init()
{
    this->setAcceptRichText(false);
    this->setWordWrapMode(QTextOption::WrapAnywhere);
    this->setViewportMargins(0, 0, 10, 0);
    // background color
    QPalette pText = this->palette();
    pText.setColor(QPalette::Base, QColor(255, 251, 225, 0));
    pText.setColor(QPalette::Text, QColor(0, 0, 0));
    this->setPalette(pText);

    DFontSizeManager::instance()->bind(this, DFontSizeManager::T8);
    this->setFrameStyle(QFrame::NoFrame);

    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextEditMaxContantNum()));
}

void TransparentTextEdit::slotTextEditMaxContantNum()
{
    QString textContent = this->toPlainText();

    int length = textContent.count();
    if (length > m_nMaxContantLen) {
        int position = this->textCursor().position();
        QTextCursor textCursor = this->textCursor();
        textContent.remove(position - (length - m_nMaxContantLen), length - m_nMaxContantLen);
        this->setText(textContent);
        textCursor.setPosition(position - (length - m_nMaxContantLen));
        this->setTextCursor(textCursor);
        sigNeedShowTips(tr("Input limit reached"), 1);
    }
}

void TransparentTextEdit::paintEvent(QPaintEvent *event)
{
    QTextEdit::paintEvent(event);
    QPainter painter(this->viewport());
    painter.setRenderHints(QPainter::Antialiasing);
    int maxLineHeight = 2;
    int totalheight = this->viewport()->height() - maxLineHeight/* + this->verticalScrollBar()->maximum()*/;
    const QFontMetricsF &fontmetricsf = QFontMetricsF(this->document()->defaultFont());
    qreal lineheight = fontmetricsf.height();
    painter.setBrush(Qt::NoBrush);
    QPen pen(QColor(219, 189, 119), maxLineHeight);
    painter.setPen(pen);
    int startLine = static_cast<int>(this->document()->documentMargin() - this->verticalScrollBar()->value());
    painter.drawLine(2, startLine, this->viewport()->width() - 4, startLine);
    pen.setWidth(1);
    painter.setPen(pen);
    qreal curh;
    for (curh = startLine + lineheight; curh <= totalheight; curh += lineheight) {
        painter.drawLine(QPointF(2.0, curh), QPointF(this->viewport()->width() * 1.0 - 4.0, curh));
    }

    if (this->verticalScrollBar()->maximum() - this->verticalScrollBar()->value() < maxLineHeight) {
        pen.setWidth(maxLineHeight);
        painter.setPen(pen);
        curh -= lineheight;
        painter.drawLine(QPointF(2.0, curh), QPointF(this->viewport()->width() * 1.0 - 4.0, curh));
    }
}

void TransparentTextEdit::insertFromMimeData(const QMimeData *source)
{
    if (!source->text().isEmpty())
        this->insertPlainText(source->text());
}

//void TransparentTextEdit::mousePressEvent(QMouseEvent *event)
//{
//    if (event->source() == Qt::MouseEventSynthesizedByQt) {
//        m_mousePos = QCursor::pos();
//        if (m_showMenuTimer) {
//            if (m_showMenuTimer->isActive()) {
//                m_showMenuTimer->stop();
//            }
//            m_showMenuTimer->start();
//        }
//    }

//    QTextEdit::mousePressEvent(event);
//}

//void TransparentTextEdit::mouseReleaseEvent(QMouseEvent *event)
//{
//    if (m_showMenuTimer) {
//        if (m_showMenuTimer->isActive()) {
//            m_showMenuTimer->stop();
//        }
//    }

//    QTextEdit::mouseReleaseEvent(event);
//}

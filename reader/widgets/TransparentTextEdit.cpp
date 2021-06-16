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
#include <QScrollBar>
#include <QTextBlock>
#include <QMimeData>
#include <QMenu>
#include <QPointF>

TransparentTextEdit::TransparentTextEdit(DWidget *parent)
    : QTextEdit(parent)
{
    this->setObjectName("TransparentTextEdit");

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

    setAttribute(Qt::WA_AcceptTouchEvents); //接受触控事件
    grabGesture(Qt::TapGesture); //获取触控点击事件
    grabGesture(Qt::TapAndHoldGesture); //获取触控点击长按事件

    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextEditMaxContantNum()));
    //滑动鼠标时选中的效果
    connect(this, &TransparentTextEdit::selectionChanged, this, &TransparentTextEdit::onSelectionArea);
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

    int totalheight = this->viewport()->height() - maxLineHeight;

    const QFontMetricsF &fontmetricsf = QFontMetricsF(this->font());

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

void TransparentTextEdit::keyPressEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_M && (keyEvent->modifiers() & Qt::AltModifier) && !keyEvent->isAutoRepeat()) {
        QMenu *menu =  this->createStandardContextMenu();

        if (menu) {
            menu->exec(this->cursor().pos());
            delete  menu;
            menu = nullptr;
        }
        return;
    }

    QTextEdit::keyPressEvent(keyEvent);
}

bool TransparentTextEdit::event(QEvent *event)
{
    if (event->type() == QEvent::Gesture)
        gestureEvent(static_cast<QGestureEvent *>(event));
    return QTextEdit::event(event);
}

void TransparentTextEdit::mousePressEvent(QMouseEvent *e)
{
    m_nSelectStart = e->pos();
    QTextEdit::mousePressEvent(e);
}

void TransparentTextEdit::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->type() == QEvent::MouseButtonRelease && e->source() == Qt::MouseEventSynthesizedByQt) {
        m_gestureAction = GA_null;
    }

    int i = m_end - m_start;
    if (Qt::MouseEventSynthesizedByQt == e->source()
            && (i > 10 && this->verticalScrollBar()->value() != 0)) {
        e->accept();
        return;
    }
    return QTextEdit::mouseReleaseEvent(e);
}

void TransparentTextEdit::mouseMoveEvent(QMouseEvent *e)
{
    if (Qt::MouseEventSynthesizedByQt == e->source()) {
        m_end = e->y();
    }

    //单指滑动
    if (e->type() == QEvent::MouseMove && e->source() == Qt::MouseEventSynthesizedByQt) {
        QPointF delta = e->pos() - m_nSelectStart;

        if (m_gestureAction == GA_slide && delta.manhattanLength() > QApplication::startDragDistance()) {
            QFont font = this->font();

            const int diffpos = e->pos().y() - m_lastMousepos;
            m_lastMouseTime = e->timestamp();
            m_lastMousepos = e->pos().y() ;

            /*开根号时数值越大衰减比例越大*/
            qreal direction = diffpos < 0 ? 1.0 : -1.0; //确定滑动方向
            slideGesture(-direction * sqrt(abs(diffpos)) / font.pointSize());

            // 如果放到外面会屏蔽掉选中
            return; //此时屏蔽其他触控
        }
    }

    if (e->type() == QEvent::MouseMove
            && e->source() != Qt::MouseEventSynthesizedByQt
            && m_gestureAction == GA_slide) {
        return;
    }

    QTextEdit::mouseMoveEvent(e);
}

bool TransparentTextEdit::gestureEvent(QGestureEvent *event)
{
    if (QGesture *tap = event->gesture(Qt::TapGesture))
        tapGestureTriggered(static_cast<QTapGesture *>(tap));
    if (QGesture *tapAndHold = event->gesture(Qt::TapAndHoldGesture))
        tapAndHoldGestureTriggered(static_cast<QTapAndHoldGesture *>(tapAndHold));
    return true;
}

void TransparentTextEdit::tapGestureTriggered(QTapGesture *tap)
{
    //单指点击函数
    switch (tap->state()) { //根据点击的状态进行不同的操作
    case Qt::GestureStarted: { //开始点击，记录时间。时间不同 进行不同的操作
        m_gestureAction = GA_tap;
        m_tapBeginTime = QDateTime::currentDateTime().toMSecsSinceEpoch();
        break;
    }
    case Qt::GestureUpdated: {
        m_gestureAction = GA_slide; //触控滑动
        break;
    }
    case Qt::GestureCanceled: {
        //根据时间长短区分轻触滑动
        qint64 timeSpace = QDateTime::currentDateTime().toMSecsSinceEpoch() - m_tapBeginTime;
        if (timeSpace < TAP_MOVE_DELAY) { //普通滑动
            m_slideContinue = true;
            m_gestureAction = GA_slide;
        } else { //选中滑动
            m_slideContinue = false;
            m_gestureAction = GA_null;
        }
        break;
    }
    case Qt::GestureFinished: {
        m_gestureAction = GA_null;
        break;
    }
    default: {
        Q_ASSERT(false);
        break;
    }
    }
}

void TransparentTextEdit::tapAndHoldGestureTriggered(QTapAndHoldGesture *tapAndHold)
{
    //单指长按
    switch (tapAndHold->state()) {
    case Qt::GestureStarted:
        m_gestureAction = GA_hold;
        break;
    case Qt::GestureUpdated:
        Q_ASSERT(false);
        break;
    case Qt::GestureCanceled:
        Q_ASSERT(false);
        break;
    case Qt::GestureFinished:
        m_gestureAction = GA_null;
        break;
    default:
        Q_ASSERT(false);
        break;
    }
}

void TransparentTextEdit::slideGesture(qreal diff)
{
    static qreal delta = 0.0;
    int step = static_cast<int>(diff + delta);
    delta = diff + delta - step;
    verticalScrollBar()->setValue(verticalScrollBar()->value() + step); //移动滚动条
}

void TransparentTextEdit::onSelectionArea()
{
    if (m_gestureAction != GA_null) {
        QTextCursor cursor = textCursor();
        if (cursor.selectedText() != "") {
            cursor.clearSelection();
            setTextCursor(cursor);
        }
    }
}

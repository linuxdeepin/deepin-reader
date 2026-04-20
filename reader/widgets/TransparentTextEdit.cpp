// Copyright (C) 2019 ~ 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TransparentTextEdit.h"
#include "Application.h"
#include "MsgHeader.h"
#include "ddlog.h"

#include <QDebug>

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
    qCDebug(appLog) << "TransparentTextEdit created, parent:" << parent;
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

    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextEditMaxContantNum()));
    qCDebug(appLog) << "TransparentTextEdit initialized";
}

void TransparentTextEdit::slotTextEditMaxContantNum()
{
    qCDebug(appLog) << "Checking text content length, max:" << m_nMaxContantLen;
    QString textContent = this->toPlainText();

    int length = textContent.count();

    if (length > m_nMaxContantLen) {
        qCDebug(appLog) << "Text length exceeds limit, trimming content";
        int position = this->textCursor().position();

        QTextCursor textCursor = this->textCursor();

        textContent.remove(position - (length - m_nMaxContantLen), length - m_nMaxContantLen);

        this->setText(textContent);

        textCursor.setPosition(position - (length - m_nMaxContantLen));

        this->setTextCursor(textCursor);

        sigNeedShowTips(tr("Input limit reached"), 1);
    }
    qCDebug(appLog) << "TransparentTextEdit::slotTextEditMaxContantNum end";
}

void TransparentTextEdit::paintEvent(QPaintEvent *event)
{
    QTextEdit::paintEvent(event);

    QPainter painter(this->viewport());
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setBrush(Qt::NoBrush);

    const qreal topLineHeight = 2.0;
    const qreal normalLineHeight = 1.0;
    const qreal leftMargin = 2.0;
    const qreal rightMargin = 4.0;
    const qreal viewWidth = this->viewport()->width() * 1.0;

    qreal scrollY = this->verticalScrollBar()->value();
    qreal docMargin = this->document()->documentMargin();
    qreal viewportHeight = this->viewport()->height();

    // Top line (thicker, at document margin)
    qreal topY = docMargin - scrollY;
    QPen pen(QColor(219, 189, 119), topLineHeight);
    painter.setPen(pen);
    painter.drawLine(QPointF(leftMargin, topY), QPointF(viewWidth - rightMargin, topY));

    // Draw ruled lines aligned with actual text layout
    pen.setWidth(normalLineHeight);
    painter.setPen(pen);

    bool atBottom = (this->verticalScrollBar()->maximum() - this->verticalScrollBar()->value()) < topLineHeight;

    QTextDocument *doc = this->document();
    QTextBlock block = doc->begin();
    while (block.isValid()) {
        QTextLayout *layout = block.layout();
        if (layout) {
            QPointF blockPos = layout->position();
            for (int i = 0; i < layout->lineCount(); ++i) {
                QTextLine textLine = layout->lineAt(i);
                qreal lineY = blockPos.y() + textLine.position().y() + textLine.height() - scrollY;
                if (lineY < topY)
                    continue;
                if (lineY > viewportHeight)
                    break;
                // Last visible line gets thicker bottom stroke
                if (atBottom && lineY + textLine.height() > viewportHeight) {
                    pen.setWidth(topLineHeight);
                    painter.setPen(pen);
                }
                painter.drawLine(QPointF(leftMargin, lineY), QPointF(viewWidth - rightMargin, lineY));
                if (pen.widthF() != normalLineHeight) {
                    pen.setWidth(normalLineHeight);
                    painter.setPen(pen);
                }
            }
        }
        block = block.next();
    }
}

void TransparentTextEdit::insertFromMimeData(const QMimeData *source)
{
    qCDebug(appLog) << "Inserting from mime data, has text:" << !source->text().isEmpty();
    if (!source->text().isEmpty())
        this->insertPlainText(source->text());
}

void TransparentTextEdit::keyPressEvent(QKeyEvent *keyEvent)
{
    // qCDebug(appLog) << "Key pressed:" << keyEvent->key() << "modifiers:" << keyEvent->modifiers();
    if (keyEvent->key() == Qt::Key_M && (keyEvent->modifiers() & Qt::AltModifier) && !keyEvent->isAutoRepeat()) {
        // qCDebug(appLog) << "Alt+M pressed, showing context menu";
        QMenu *menu =  this->createStandardContextMenu();

        if (menu) {
            // qCDebug(appLog) << "Showing context menu";
            menu->exec(this->cursor().pos());
            delete  menu;
            menu = nullptr;
        }
        return;
    }

    QTextEdit::keyPressEvent(keyEvent);
    // qCDebug(appLog) << "TransparentTextEdit::keyPressEvent end";
}

void TransparentTextEdit::focusOutEvent(QFocusEvent *event)
{
    // qCDebug(appLog) << "Focus out event, reason:" << event->reason();
    QTextEdit::focusOutEvent(event);

    Q_EMIT sigCloseNoteWidget();
    // qCDebug(appLog) << "TransparentTextEdit::focusOutEvent end";
}

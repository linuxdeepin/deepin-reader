/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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

#include <DFontSizeManager>

#include "application.h"
#include "ModuleHeader.h"
#include "MsgHeader.h"

TransparentTextEdit::TransparentTextEdit(DWidget *parent)
    : QTextEdit(parent)
{
    int tW = 205;
    int tH = 257;
    dApp->adaptScreenView(tW, tH);
    setFixedSize(tW, tH);
    init();
}

void TransparentTextEdit::init()
{
    // background color
    QPalette pText = this->palette();
    pText.setColor(QPalette::Base, QColor(255, 251, 225));
    this->setPalette(pText);

    // font
    DFontSizeManager::instance()->bind(this, DFontSizeManager::T8);

    // text corlor
    this->setTextColor(QColor(QString("#452B0A")));

    // widgets style
    this->setFrameStyle(QFrame::NoFrame);

    // text under line
    QTextCursor cursor(this->textCursor());
    QTextCharFormat format = cursor.charFormat();
    QTextBlockFormat textBlockFormat;
    // line height
    textBlockFormat.setLineHeight(19, QTextBlockFormat::FixedHeight);
    // line margin
    textBlockFormat.setBottomMargin(0);
    cursor.mergeCharFormat(format);
    cursor.setBlockFormat(textBlockFormat);
    this->setTextCursor(cursor);
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    connect(this, SIGNAL(textChanged()), this, SLOT(slotTextEditMaxContantNum()));
}

/**
 * @brief CustemTextEdit::slotTextEditMaxContantNum
 * TextEdit输入允许输入最长字符串的长度
 */
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

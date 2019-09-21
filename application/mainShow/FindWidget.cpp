/* -*- Mode: C++; indent-tabs-mode: nil; tab-width: 4 -*-
 * -*- coding: utf-8 -*-
 *
 * Copyright (C) 2011 ~ 2018 Deepin, Inc.
 *
 * Author:     Wang Yong <wangyong@deepin.com>
 * Maintainer: Rekols    <rekols@foxmail.com>
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

#include "FindWidget.h"

FindWidget::FindWidget(CustomWidget *parent)
    : CustomWidget("FindWidget", parent)
{
    setFixedSize(QSize(410, 50));

    initWidget();

    setVisible(false);
}

void FindWidget::findCancel()
{
    this->close();
}

void FindWidget::handleContentChanged()
{
    QString strNewFind = m_pSearchEdit->text();
    if (strNewFind != m_strOldFindContent) {
        m_strOldFindContent = strNewFind;
        sendMsg(MSG_FIND_CONTENT, m_strOldFindContent);
    }
}

void FindWidget::slotFindNextBtnClicked()
{
    sendMsg(MSG_FIND_NEXT);
}

void FindWidget::slotFindPrevBtnClicked()
{
    sendMsg(MSG_FIND_PREV);
}

//  清空搜索内容
void FindWidget::slotClearContent()
{
    m_strOldFindContent = "";
    sendMsg(MSG_CLEAR_FIND_CONTENT);
}

void FindWidget::hideEvent(QHideEvent *e)
{
    m_strOldFindContent = "";
    sendMsg(MSG_CLEAR_FIND_CONTENT);

    CustomWidget::hideEvent(e);
}

int FindWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void FindWidget::initWidget()
{
    DPushButton *findNextButton = new DPushButton("d");
    findNextButton->setFixedSize(QSize(36, 36));
    connect(findNextButton, &DPushButton::clicked, this, &FindWidget::slotFindNextBtnClicked);

    DPushButton *findPrevButton = new DPushButton("u");
    findPrevButton->setFixedSize(QSize(36, 36));
    connect(findPrevButton, &DPushButton::clicked, this, &FindWidget::slotFindPrevBtnClicked);

    DImageButton *closeButton = new DImageButton;
    closeButton->setFixedSize(20, 20);
    connect(closeButton, &DImageButton::clicked, this, &FindWidget::findCancel);

    m_pSearchEdit = new DSearchEdit;
    connect(m_pSearchEdit, &DSearchEdit::returnPressed, this, &FindWidget::handleContentChanged);
    connect(m_pSearchEdit, &DSearchEdit::textChanged, this, &FindWidget::slotClearContent);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_pSearchEdit);
    layout->addWidget(findNextButton);
    layout->addWidget(findPrevButton);
    layout->addWidget(closeButton);
}

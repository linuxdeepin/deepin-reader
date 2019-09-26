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
#include <DFloatingButton>
#include <QHBoxLayout>
#include "translator/MainShow.h"

FindWidget::FindWidget(CustomWidget *parent)
    : CustomWidget("FindWidget", parent)
{
    setFixedSize(QSize(410, 40));
    setWindowFlags(windowFlags() |  Qt::WindowStaysOnTopHint);

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
    this->raise();
}

void FindWidget::slotFindPrevBtnClicked()
{
    sendMsg(MSG_FIND_PREV);
    this->raise();
}

//  清空搜索内容
void FindWidget::slotClearContent()
{
    QString strNewFind = m_pSearchEdit->text();
    if (strNewFind == "") {
        m_strOldFindContent = "";
        sendMsg(MSG_CLEAR_FIND_CONTENT);
    }
}

void FindWidget::hideEvent(QHideEvent *e)
{
    m_strOldFindContent = "";
    sendMsg(MSG_CLEAR_FIND_CONTENT);

    CustomWidget::hideEvent(e);
}

int FindWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更

    }
    return 0;
}

void FindWidget::initWidget()
{
    DFloatingButton *findNextButton = new DFloatingButton(DStyle::SP_ArrowNext);
    findNextButton->setToolTip(MainShow::NEXT_ONE);
    findNextButton->setFixedSize(QSize(30, 30));
    connect(findNextButton, &DFloatingButton::clicked, this, &FindWidget::slotFindNextBtnClicked);

    DFloatingButton *findPrevButton = new DFloatingButton(DStyle::SP_ArrowPrev);
    findNextButton->setToolTip(MainShow::PREV_ONE);
    findPrevButton->setFixedSize(QSize(30, 30));
    connect(findPrevButton, &DFloatingButton::clicked, this, &FindWidget::slotFindPrevBtnClicked);

    DFloatingButton *closeButton = new DFloatingButton(DStyle::SP_CloseButton);
    closeButton->setFixedSize(QSize(30, 30));
    connect(closeButton, &DFloatingButton::clicked, this, &FindWidget::findCancel);

    m_pSearchEdit = new DSearchEdit;

    connect(m_pSearchEdit, &DSearchEdit::returnPressed, this, &FindWidget::handleContentChanged);
    connect(m_pSearchEdit, &DSearchEdit::textChanged, this, &FindWidget::slotClearContent);

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->addWidget(m_pSearchEdit);
    layout->addWidget(findNextButton);
    layout->addWidget(findPrevButton);
    layout->addWidget(closeButton);
}

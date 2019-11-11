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
#include <QHBoxLayout>
#include <DIconButton>
#include <DDialogCloseButton>
#include "subjectObserver/MsgHeader.h"

FindWidget::FindWidget(DWidget *parent)
    : DFloatingWidget(parent)
{
    setFixedSize(QSize(410, 40));
    setWindowFlags(windowFlags() |  Qt::WindowStaysOnTopHint);

    initWidget();

    setVisible(false);

    m_pNotifySubject = NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
    }
}

FindWidget::~FindWidget()
{
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }

    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
    }
}

void FindWidget::findCancel()
{
    notifyMsg(MSG_CLEAR_FIND_CONTENT);
    m_pSearchEdit->clear();
    m_pSearchEdit->clearFocus();
    hide();
}

void FindWidget::handleContentChanged()
{
    QString strNewFind = m_pSearchEdit->text();
    if (strNewFind != m_strOldFindContent) {
        m_strOldFindContent = strNewFind;
        notifyMsg(MSG_FIND_CONTENT, m_strOldFindContent);
    }
}

void FindWidget::slotFindNextBtnClicked()
{
    notifyMsg(MSG_FIND_NEXT);
    this->raise();
}

void FindWidget::slotFindPrevBtnClicked()
{
    notifyMsg(MSG_FIND_PREV);
    this->raise();
}

//  清空搜索内容
void FindWidget::slotClearContent()
{
    QString strNewFind = m_pSearchEdit->text();
    if (strNewFind == "") {
        m_strOldFindContent = "";
        notifyMsg(MSG_CLEAR_FIND_CONTENT);
    }
}

void FindWidget::hideEvent(QHideEvent *e)
{
    m_strOldFindContent = "";
    DFloatingWidget::hideEvent(e);
}

int FindWidget::dealWithData(const int &msgType, const QString &)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更
    } else if ( msgType == MSG_OPERATION_SLIDE) {   //  幻灯片了
        this->setVisible(false);
    }
    return 0;
}

void FindWidget::sendMsg(const int &msgType, const QString &msgContent)
{
    m_pMsgSubject->sendMsg(msgType, msgContent);
}

void FindWidget::notifyMsg(const int &msgType, const QString &msgContent)
{
    m_pNotifySubject->notifyMsg(msgType, msgContent);
}

void FindWidget::initWidget()
{
    auto findNextButton = new DIconButton(DStyle::SP_ArrowDown);
    findNextButton->setToolTip(tr("next one"));
    findNextButton->setFixedSize(QSize(30, 30));
    connect(findNextButton, &DIconButton::clicked, this, &FindWidget::slotFindNextBtnClicked);

    auto findPrevButton = new DIconButton(DStyle::SP_ArrowUp);
    findNextButton->setToolTip(tr("prev one"));
    findPrevButton->setFixedSize(QSize(30, 30));
    connect(findPrevButton, &DIconButton::clicked, this, &FindWidget::slotFindPrevBtnClicked);

    auto closeButton = new DDialogCloseButton;
    closeButton->setIconSize(QSize(30, 30));
    connect(closeButton, &DDialogCloseButton::clicked, this, &FindWidget::findCancel);

    m_pSearchEdit = new DSearchEdit;
    m_pSearchEdit->setFocusPolicy(Qt::StrongFocus);

    connect(m_pSearchEdit, &DSearchEdit::returnPressed, this, &FindWidget::handleContentChanged);
    connect(m_pSearchEdit, &DSearchEdit::textChanged, this, &FindWidget::slotClearContent);

    auto layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->addWidget(m_pSearchEdit);
    layout->addWidget(findNextButton);
    layout->addWidget(findPrevButton);
    layout->addWidget(closeButton);
}

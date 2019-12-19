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
#include <DDialogCloseButton>
#include <DIconButton>
#include <QDebug>
#include <QHBoxLayout>
#include "application.h"
#include "controller/DataManager.h"
#include "subjectObserver/ModuleHeader.h"
#include "subjectObserver/MsgHeader.h"
#include "frame/DocummentFileHelper.h"

FindWidget::FindWidget(DWidget *parent)
    : DFloatingWidget(parent)
{
    setMinimumSize(QSize(414, 60));
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setBlurBackgroundEnabled(true);

    initWidget();
    initConnection();

    slotSetVisible();

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

void FindWidget::showPosition(const int &nParentWidth)
{
    int nWidget = this->width();
    this->move(nParentWidth - nWidget - 20, 20);
    this->show();
    this->raise();
}

void FindWidget::slotSetVisible()
{
    this->setVisible(false);
}

void FindWidget::findCancel()
{
    slotFindNone(0);
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
    //notifyMsg(MSG_FIND_NEXT);
    DocummentFileHelper::instance()->findNext();
    this->raise();
}

void FindWidget::slotFindPrevBtnClicked()
{
    // notifyMsg(MSG_FIND_PREV);
    DocummentFileHelper::instance()->findPrev();
    this->raise();
}

//  清空搜索内容
void FindWidget::slotClearContent()
{
    QString strNewFind = m_pSearchEdit->text();
    if (strNewFind == "") {
        m_strOldFindContent = "";
        slotFindNone(0);
        notifyMsg(MSG_CLEAR_FIND_CONTENT);
    }
}

/**
 * @brief FindWidget::slotFindNone
 * 搜索无果时，搜索框变成粉红色
 * @param status
 * 0:恢复正常颜色 1:变成粉红色
 */
void FindWidget::slotFindNone(int status)
{
    bool alert = false;

    alert = (status == 1) ? true : false;

    if (m_pSearchEdit) {
        m_pSearchEdit->setAlert(alert);
    }
}

void FindWidget::hideEvent(QHideEvent *e)
{
    m_strOldFindContent = "";
    DFloatingWidget::hideEvent(e);
}

int FindWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_UPDATE_THEME) {  //  主题变更
    } else if (msgType == MSG_OPERATION_SLIDE) {  //  幻灯片了
        emit sigSetVisible();
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == KeyStr::g_f11) {
            emit sigSetVisible();
        }
    } else if (msgType == MSG_FIND_NONE) {
        emit sigFindNone(1);
        return ConstantMsg::g_effective_res;
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
    findNextButton->setFixedSize(QSize(36, 36));
    findNextButton->setIconSize(QSize(12, 12));
    connect(findNextButton, &DIconButton::clicked, this, &FindWidget::slotFindNextBtnClicked);

    auto findPrevButton = new DIconButton(DStyle::SP_ArrowUp);
    findPrevButton->setToolTip(tr("prev one"));
    findPrevButton->setFixedSize(QSize(36, 36));
    findPrevButton->setIconSize(QSize(12, 12));
    connect(findPrevButton, &DIconButton::clicked, this, &FindWidget::slotFindPrevBtnClicked);

    auto closeButton = new DDialogCloseButton;
    closeButton->setIconSize(QSize(28, 28 /*20, 20*/));
    closeButton->setFixedSize(QSize(30, 30 /*22, 22*/));
    connect(closeButton, &DDialogCloseButton::clicked, this, &FindWidget::findCancel);

    m_pSearchEdit = new DSearchEdit;
    m_pSearchEdit->setFixedSize(QSize(270, 36));
    m_pSearchEdit->setFocusPolicy(Qt::StrongFocus);

    connect(m_pSearchEdit, &DSearchEdit::returnPressed, this, &FindWidget::handleContentChanged);
    connect(m_pSearchEdit, &DSearchEdit::textChanged, this, &FindWidget::slotClearContent);

    auto layout = new QHBoxLayout;
    layout->setContentsMargins(8, 0, 6, 0);
    layout->addWidget(m_pSearchEdit);
    layout->addWidget(findPrevButton);
    layout->addWidget(findNextButton);
    layout->addWidget(closeButton);
    this->setLayout(layout);
}

void FindWidget::initConnection()
{
    connect(this, SIGNAL(sigSetVisible()), SLOT(slotSetVisible()));
    connect(this, SIGNAL(sigFindNone(int)), this, SLOT(slotFindNone(int)));
}

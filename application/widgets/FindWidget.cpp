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
#include <QDesktopWidget>
#include <QJsonObject>
#include <QJsonDocument>

#include "docview/docummentproxy.h"
#include "widgets/main/MainTabWidgetEx.h"

FindWidget::FindWidget(DWidget *parent)
    : DFloatingWidget(parent)
{
    m_strObserverName = FIND_WIDGET;

    setMinimumSize(QSize(414, 60));
    setWindowFlags(Qt::WindowStaysOnTopHint);
    setBlurBackgroundEnabled(true);

    m_pMsgList = {MSG_FIND_NONE_CONTENT};
    initWidget();
    initConnection();

    this->setVisible(false);

    dApp->m_pModelService->addObserver(this);
}

FindWidget::~FindWidget()
{
    dApp->m_pModelService->removeObserver(this);
}

void FindWidget::showPosition(const int &nParentWidth)
{
    int nWidget = this->width();
    this->move(nParentWidth - nWidget - 20, 20);
    this->show();
    this->raise();
}

void FindWidget::setSearchEditFocus()
{
    m_pSearchEdit->lineEdit()->setFocus();
}

void FindWidget::findCancel()
{
    m_pSearchEdit->setText("");
    slotEditAborted();

    this->close();
}

void FindWidget::handleContentChanged()
{
    QString strFind = m_pSearchEdit->text();
    if (strFind != "") {
        onSetEditAlert(0);
        emit sigFindOperation(E_FIND_CONTENT, strFind);
    }
}

void FindWidget::slotFindNextBtnClicked()
{
    emit sigFindOperation(E_FIND_NEXT, "");
}

void FindWidget::slotFindPrevBtnClicked()
{
    emit sigFindOperation(E_FIND_PREV, "");
}

//  文本内容变化, 为空, 则取消红色提示
void FindWidget::slotClearContent()
{
    QString strNewFind = m_pSearchEdit->text();
    if (strNewFind == "") {
        onSetEditAlert(0);
    }
}

//  点击 搜索框 里面 的 x
void FindWidget::slotEditAborted()
{
//    qDebug() << __FUNCTION__;
    onSetEditAlert(0);
    emit sigFindOperation(E_FIND_EXIT, "");
}

int FindWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_FIND_NONE_CONTENT) {
        onSetEditAlert(1);
    }

    if (m_pMsgList.contains(msgType)) {
        return MSG_OK;
    }
    return MSG_NO_OK;
}

void FindWidget::notifyMsg(const int &msgType, const QString &msgContent)
{
    if (this->isVisible()) {
        dApp->m_pModelService->notifyMsg(msgType, msgContent);
    }
}

void FindWidget::initWidget()
{
    auto findNextButton = new DIconButton(DStyle::SP_ArrowDown);
    findNextButton->setToolTip(tr("Next"));
    findNextButton->setFixedSize(QSize(36, 36));
    findNextButton->setIconSize(QSize(12, 12));
    connect(findNextButton, &DIconButton::clicked, this, &FindWidget::slotFindNextBtnClicked);

    auto findPrevButton = new DIconButton(DStyle::SP_ArrowUp);
    findPrevButton->setToolTip(tr("Previous"));
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
    connect(m_pSearchEdit,  &DSearchEdit::searchAborted, this, &FindWidget::slotEditAborted);

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
}

////  退出查询
//void FindWidget::onFindExit()
//{
//    this->setVisible(false);

//    if (m_pSearchEdit) {
//        m_pSearchEdit->setText("");
//    }
//}

//  设置 提醒红色
void FindWidget::onSetEditAlert(const int &iFlag)
{
    if (m_pSearchEdit) {
        bool bAlert = iFlag == 1 ? true : false;
        m_pSearchEdit->setAlert(bAlert);
    }
}

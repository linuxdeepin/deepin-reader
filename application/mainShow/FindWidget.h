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

#ifndef FINDWIDGET_H
#define FINDWIDGET_H

#include <DWidget>
#include <DFloatingWidget>
#include <DSearchEdit>

#include "subjectObserver/IObserver.h"
#include "controller/MsgSubject.h"
#include "controller/NotifySubject.h"

DWIDGET_USE_NAMESPACE

class FindWidget : public DFloatingWidget, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(FindWidget)

public:
    explicit FindWidget(DWidget *parent = nullptr);
    ~FindWidget() Q_DECL_OVERRIDE;

signals:
    void sigSetVisible();

public:
    void showPosition(const int &);

private slots:
    void slotSetVisible();
    void findCancel();
    void handleContentChanged();
    void slotFindNextBtnClicked();
    void slotFindPrevBtnClicked();
    void slotClearContent();

protected:
    void hideEvent(QHideEvent *event) Q_DECL_OVERRIDE;

private:
    void initWidget();
    void initConnection();

private:
    DSearchEdit     *m_pSearchEdit = nullptr;
    QString         m_strOldFindContent = "";
    NotifySubject   *m_pNotifySubject = nullptr;
    MsgSubject      *m_pMsgSubject = nullptr;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;
};

#endif

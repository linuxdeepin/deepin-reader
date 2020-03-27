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

#include "application.h"

DWIDGET_USE_NAMESPACE

class FindWidget : public DFloatingWidget, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(FindWidget)

public:
    explicit FindWidget(DWidget *parent = nullptr);
    ~FindWidget() override;

signals:
    void sigFindOperation(const int &, const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    void notifyMsg(const int &, const QString &msgContent = "") override;

public:
    void showPosition(const int &);
    void setSearchEditFocus();

private slots:
    void findCancel();
    void handleContentChanged();
    void slotFindNextBtnClicked();
    void slotFindPrevBtnClicked();
    void slotClearContent();
    void slotEditAborted();

private:
    void initWidget();
    void initConnection();

//    void onFindExit();
    void onSetEditAlert(const int &iFlag);

private:
    DSearchEdit     *m_pSearchEdit = nullptr;

    QList<int>      m_pMsgList;
    QString m_strLastFindText{""};//上一次查找内容
};

#endif

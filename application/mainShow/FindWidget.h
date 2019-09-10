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

#include "LineBar.h"

#include <QHBoxLayout>
#include <DPushButton>
#include <DLabel>
#include <QPainter>
#include <DImageButton>
#include <QColor>

#include "subjectObserver/CustomWidget.h"

class FindWidget : public CustomWidget
{
    Q_OBJECT

public:
    FindWidget(CustomWidget *parent = nullptr);

    bool isFocus();
    void focus();

    void setMismatchAlert(bool isAlert);

    void setBackground(QString color);

private slots:
    void findCancel();
    void handleContentChanged();
    void slotFindNextBtnClicked();
    void slotFindPrevBtnClicked();

protected:
    void paintEvent(QPaintEvent *event);
    void hideEvent(QHideEvent *event);

private:
    DImageButton *m_closeButton;
    LineBar *m_editLine;
    QHBoxLayout *m_layout;

    QColor m_backgroundColor;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &);

    // CustomWidget interface
protected:
    void initWidget();
};

#endif

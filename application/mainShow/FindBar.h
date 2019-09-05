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

#ifndef FINDBAR_H
#define FINDBAR_H

#include "LineBar.h"

#include <QHBoxLayout>
#include <DPushButton>
#include <DLabel>
#include <QPainter>
#include "dimagebutton.h"
#include <QColor>

#include "subjectObserver/CustomWidget.h"

class FindBar : public CustomWidget
{
    Q_OBJECT

public:
    FindBar(CustomWidget *parent = nullptr);

    bool isFocus();
    void focus();

    void activeInput(QString text, QString file, int row, int column, int scrollOffset);
    void setMismatchAlert(bool isAlert);

    void setBackground(QString color);

signals:
    void findNext();
    void findPrev();

    void removeSearchKeyword();
    void updateSearchKeyword(QString file, QString keyword);

    //add guoshao
    void sigFindbarCancel();

public slots:
    void findCancel();
    void handleContentChanged();

protected:
    void paintEvent(QPaintEvent *event);
    void hideEvent(QHideEvent *event);

private:
    DPushButton *m_findNextButton;
    DPushButton *m_findPrevButton;
    DImageButton *m_closeButton;
    LineBar *m_editLine;
    QHBoxLayout *m_layout;
    DLabel *m_findLabel;
    QString m_findFile;
    int m_findFileColumn;
    int m_findFileRow;
    int m_findFileSrollOffset;
    QColor m_backgroundColor;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &);

    // CustomWidget interface
protected:
    void initWidget();
};

#endif

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

#include <QDebug>

FindWidget::FindWidget(CustomWidget *parent)
    : CustomWidget("FindWidget", parent)
{
    // Init.
    setWindowFlags(Qt::FramelessWindowHint | Qt::X11BypassWindowManagerHint);
    setFixedSize(QSize(410, 50));

    // Init layout and mainShow.
    m_layout = new QHBoxLayout(this);

    //modify by guoshaoyu
    m_editLine = new LineBar();

    initWidget();

    connect(m_editLine, &LineBar::pressEsc, this, &FindWidget::findCancel);
    connect(m_editLine, &LineBar::pressEnter, this, &FindWidget::handleContentChanged);
    connect(m_editLine, &LineBar::pressCtrlEnter, this, &FindWidget::slotFindPrevBtnClicked);
    //connect(m_editLine, &LineBar::contentChanged, this, &FindWidget::handleContentChanged, Qt::QueuedConnection);

    setVisible(false);
}

//bool FindWidget::isFocus()
//{
//    return m_editLine->hasFocus();
//}

//void FindWidget::focus()
//{
//    m_editLine->setFocus();
//    m_editLine->selectAll();
//}

void FindWidget::findCancel()
{
    QWidget::hide();
}

void FindWidget::handleContentChanged()
{
    QString strNewFind = m_editLine->text();
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

void FindWidget::hideEvent(QHideEvent *)
{
    sendMsg(MSG_CLEAR_FIND_CONTENT);
}

int FindWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void FindWidget::initWidget()
{
    DPushButton *findNextButton = new DPushButton("u");
    findNextButton->setFixedSize(QSize(36, 36));
    connect(findNextButton, &DPushButton::clicked, this, &FindWidget::slotFindNextBtnClicked);

    DPushButton *findPrevButton = new DPushButton("d");
    findPrevButton->setFixedSize(QSize(36, 36));
    connect(findPrevButton, &DPushButton::clicked, this, &FindWidget::slotFindPrevBtnClicked);

    m_closeButton = new DImageButton();
    m_closeButton->setFixedSize(20, 20);
    connect(m_closeButton, &DImageButton::clicked, this, &FindWidget::findCancel);

    m_layout->addWidget(m_editLine);
    m_layout->addWidget(findNextButton);
    m_layout->addWidget(findPrevButton);
    m_layout->addWidget(m_closeButton);

    // Make button don't grab keyboard focus after click it.
//    findNextButton->setFocusPolicy(Qt::NoFocus);
//    findPrevButton->setFocusPolicy(Qt::NoFocus);
//    m_closeButton->setFocusPolicy(Qt::NoFocus);
}

//void FindWidget::paintEvent(QPaintEvent *)
//{
//    QPainter painter(this);
//    painter.setOpacity(1);
//    QPainterPath path;
//    path.addRect(rect());
//    painter.fillPath(path, m_backgroundColor);
//}

//void FindWidget::setMismatchAlert(bool isAlert)
//{
//    m_editLine->setAlert(isAlert);
//}

//void FindWidget::setBackground(QString color)
//{
//    m_backgroundColor = QColor(color);

//    if (QColor(m_backgroundColor).lightness() < 128) {
//        //m_findLabel->setStyleSheet(QString("QLabel { background-color: %1; color: %2; }").arg(color).arg("#AAAAAA"));

//        m_closeButton->setNormalPic(Utils::getQrcPath("bar_close_normal_dark.svg"));
//        m_closeButton->setHoverPic(Utils::getQrcPath("bar_close_hover_dark.svg"));
//        m_closeButton->setPressPic(Utils::getQrcPath("bar_close_press_dark.svg"));
//    } else {
//        //m_findLabel->setStyleSheet(QString("QLabel { background-color: %1; color: %2; }").arg(color).arg("#000000"));

//        m_closeButton->setNormalPic(Utils::getQrcPath("bar_close_normal_light.svg"));
//        m_closeButton->setHoverPic(Utils::getQrcPath("bar_close_hover_light.svg"));
//        m_closeButton->setPressPic(Utils::getQrcPath("bar_close_press_light.svg"));
//    }

//    update();
//}

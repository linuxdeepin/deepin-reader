/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     duanxiaohui<duanxiaohui@uniontech.com>
 *
 * Maintainer: duanxiaohui<duanxiaohui@uniontech.com>
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
#include "BookMarkButton.h"
#include "Utils.h"

#include<DApplicationHelper>

#include<QColor>
#include<QBrush>
#include<QPen>
#include<QPoint>
#include<QPainter>
#include<QPaintEvent>
#include<QMouseEvent>

DWIDGET_USE_NAMESPACE

BookMarkButton::BookMarkButton(DWidget *parent)
    : DPushButton(parent),
      ishovered(false),
      ispressed(false),
      isclicked(false)
{
    setFixedSize(QSize(39, 39));
    setMouseTracking(true);
}

bool BookMarkButton::clickState()
{
    return isclicked;
}
void BookMarkButton::setClickState(bool state)
{
    isclicked = state;
    update();
}

void BookMarkButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QString iconPath;
    QPixmap pixmap;
    if (isclicked) {
        pixmap = QIcon::fromTheme("dr_bookmark_checked").pixmap(QSize(39, 39));
    } else {
        if (ispressed) {
            pixmap = QIcon::fromTheme("dr_bookmark_pressed").pixmap(QSize(39, 39));
        } else {
            if (ishovered) {
                pixmap = QIcon::fromTheme("dr_bookmark_hover").pixmap(QSize(39, 39));
            }
        }
    }

    painter.drawPixmap(0, 0, this->width(), this->height(), pixmap);
}

void BookMarkButton::enterEvent(QEvent *)
{
    ishovered = true;
    repaint();
}

void BookMarkButton::leaveEvent(QEvent *)
{
    ishovered = false;
    repaint();
}


void BookMarkButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        ispressed = true;
        repaint();
    }
}

void BookMarkButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        ispressed = false;

        emit sigClicked(!isclicked);

        repaint();
    }
}

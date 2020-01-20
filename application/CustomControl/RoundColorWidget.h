/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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

#ifndef CUSTOMCONTROL_ROUNDCOLORWIDGET_H
#define CUSTOMCONTROL_ROUNDCOLORWIDGET_H

#include <DWidget>

DWIDGET_USE_NAMESPACE

class RoundColorWidget : public DWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(RoundColorWidget)

public:
    explicit RoundColorWidget(const QColor &color, QWidget *parent = nullptr);
    bool isSelected();
    void setSelected(bool selected);
    static const int EXTRA = 2; //2px extra space to avoid line cutted off

Q_SIGNALS:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    bool m_isSelected = false;
    QColor m_color;
};

#endif // CUSTOMCONTROL_ROUNDCOLORWIDGET_H

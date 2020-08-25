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
#ifndef CUSTOMCLICKLABEL_H
#define CUSTOMCLICKLABEL_H

#include <DWidget>
#include <DLabel>
#include <DGuiApplicationHelper>

#include <QMouseEvent>

DWIDGET_USE_NAMESPACE

class CustomClickLabel : public DLabel
{
    Q_OBJECT
    Q_DISABLE_COPY(CustomClickLabel)

public:
    CustomClickLabel(const QString &text = "", DWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CUSTOMCLICKLABEL_H

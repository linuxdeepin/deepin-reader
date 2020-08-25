/*
 * Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     wangzhixuan<wangzhixuan@uniontech.com>
 *
 * Maintainer: wangzhixuan<wangzhixuan@uniontech.com>
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
#ifndef SPINNERWIDGET_H
#define SPINNERWIDGET_H

#include <DWidget>

DWIDGET_USE_NAMESPACE

class SpinnerWidget : public DWidget
{
    Q_OBJECT
public:
    explicit SpinnerWidget(DWidget *parent = nullptr);

public:
    void startSpinner();
    void stopSpinner();
    void setSpinnerSize(const QSize &s);

private:
    void InitSpinner();
};

#endif // SPINNERWIDGET_H

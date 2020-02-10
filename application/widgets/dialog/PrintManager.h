/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
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
#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <QObject>

#include <DWidget>

class QPrinter;

DWIDGET_USE_NAMESPACE


/**
 * @brief The PrintManager class
 * @brief   打印管理
 *
 */


class PrintManager : public QObject
{
    Q_OBJECT
public:
    explicit PrintManager(QObject *parent = nullptr);

    void showPrintDialog(DWidget *widget);

private slots:
    void slotPrintPreview(QPrinter *printer);
};

#endif // PRINTMANAGER_H

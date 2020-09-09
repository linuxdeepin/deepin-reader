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
#ifndef PRINTMANAGER_H
#define PRINTMANAGER_H

#include <DWidget>

#include <QObject>

//#include "dprintpreviewdialog.h"

#include <QPrintPreviewDialog>

class QPrinter;

DWIDGET_USE_NAMESPACE

class DocSheet;
class PrintManager : public QObject
{
    Q_OBJECT
public:
    explicit PrintManager(DocSheet *sheet, QObject *parent = nullptr);

    void showPrintDialog(DWidget *widget);

    void setPrintPath(const QString &strPrintPath);

private slots:
    void slotPrintPreview(QPrinter *printer);

private:
    QString     m_strPrintName = "";
    QString     m_strPrintPath = "";
    DocSheet   *m_sheet;
};

#endif // PRINTMANAGER_H

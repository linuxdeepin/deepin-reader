/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
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
#ifndef PROXYFILEDATAMODEL_H
#define PROXYFILEDATAMODEL_H

#include <QObject>

#include "FileDataModel.h"

class SheetBrowserPDFPrivate;

class ProxyFileDataModel : public QObject
{
    Q_OBJECT
public:
    explicit ProxyFileDataModel(QObject *parent = nullptr);

public:
    void saveOper();
    void saveData();
    void saveAsData(const QString &originPath, const QString &targetPath);

    void setModel(FileDataModel model);

private:
    SheetBrowserPDFPrivate *_fvwParent = nullptr;
    friend class SheetBrowserPDFPrivate;
};

#endif // PROXYFILEDATAMODEL_H

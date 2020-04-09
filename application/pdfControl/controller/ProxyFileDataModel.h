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
    void setData(const int &, const QString &);
    void saveData();

    FileDataModel qGetFileData() const;
    void qSetFileData(const FileDataModel &);
private:
    void setThumbnailState(const QString &);
    void SetLeftWidgetIndex(const QString &sValue);
    void OnSetViewScale(const QString &);
    void OnSetViewRotate(const QString &);
    void OnSetCurPage(const QString &sValue);

private:
    FileDataModel           m_pFileDataModel;        //  已打开的文档列表
    SheetBrowserPDFPrivate *_fvwParent = nullptr;
    friend class SheetBrowserPDFPrivate;
};

#endif // PROXYFILEDATAMODEL_H

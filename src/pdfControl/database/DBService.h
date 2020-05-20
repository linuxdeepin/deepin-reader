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
#ifndef DBSERVICE_H
#define DBSERVICE_H

#include "dbservice_global.h"

#include <QObject>

#include "FileDataModel.h"

enum E_DB_TYPE {
    DB_BOOKMARK,
    DB_HISTROY
};

class DBFactory;

class DBSERVICESHARED_EXPORT DBService : public QObject
{

public:
    explicit DBService(QObject *parent = nullptr);

public:
    void qSelectData(const QString &, const int &);
    void qSaveData(const QString &sPath, const int &);

    void saveAsData(const QString &originPath, const QString &targetPath, const int &);

    QList<int> getBookMarkList(const QString &) const;
    void setBookMarkList(const QString &, const QList<int> &pBookMarkList);

    FileDataModel getHistroyData(const QString &) const;
    void setHistroyData(const QString &, const int &, const QVariant &);

private:
    DBFactory   *m_pBookMark = nullptr;
    DBFactory   *m_pHistroy = nullptr;
};

#endif // DBSERVICE_H

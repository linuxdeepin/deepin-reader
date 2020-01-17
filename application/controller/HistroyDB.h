/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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
#ifndef CONTROLLER_HISTROYDB_H
#define CONTROLLER_HISTROYDB_H

// FileFontTable
////////////////////////////////////////////////////////////////////////
//FilePath           | FileScale | FileDoubPage | FileFit | FileRotate//
//TEXT primari key   | TEXT      | TEXT         | TEXT    | TEXT      //
////////////////////////////////////////////////////////////////////////

#include "DBManager.h"

class HistroyDB : public DBManager
{
    Q_OBJECT
    Q_DISABLE_COPY(HistroyDB)

public:
    explicit HistroyDB(DBManager *parent = nullptr);

public:

    //FileFontTable
    void insertFileFontMsg(const QString &, const QString &, const QString &, const QString &, const QString filePath = "");
    void updateFileFontMsg(const QString &, const QString &, const QString &, const QString &, const QString filePath = "");
    void deleteFileFontMsg();
    void saveFileFontMsg();
    bool saveAsFileFontMsg(const QString &, const QString &, const QString &, const QString &, const QString newFilePath = "");
    void getFileFontMsg(QString &, QString &, QString &, QString &, const QString &);
};

#endif // CONTROLLER_HISTROYDB_H

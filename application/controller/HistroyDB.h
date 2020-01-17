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

// FilesTable
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//FilePath           | FileScale | FileDoubPage | FileFit  | FileRotate | FileShowLeft | ListIndex | CurPage //
//TEXT primari key   | TEXT      | TEXT         | TEXT     | TEXT       | TEXT         | TEXT      | TEXT    //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <QObject>

#include "DBManager.h"
#include "DataManager.h"

class HistroyDB : public DBManager
{
    Q_OBJECT
    Q_DISABLE_COPY(HistroyDB)

public:
    explicit HistroyDB(QObject *parent = nullptr);

private:
    void checkDatabases();
    void clearInvalidRecords();

public:
    //FilesTable
    void insertFileFontMsg(const st_fileHistoryMsg &, const QString filePath = "");
    void updateFileFontMsg(const st_fileHistoryMsg &, const QString filePath = "");
    void saveFileFontMsg(const st_fileHistoryMsg &, const QString filePath = "");
    void saveAsFileFontMsg(const st_fileHistoryMsg &, const QString filePath = "");
    void getFileFontMsg(st_fileHistoryMsg &, const QString filePath = "");
};

#endif // CONTROLLER_HISTROYDB_H

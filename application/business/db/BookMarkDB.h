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
#ifndef BOOKMARKDB_H
#define BOOKMARKDB_H

// BookMarkTable
///////////////////////////////////////////////////////
//FilePath           | FileName | PageNumber       //
//TEXT primari key   | TEXT     | TEXT        //
///////////////////////////////////////////////////////

#include "DBFactory.h"

class BookMarkDB : public DBFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(BookMarkDB)

public:
    explicit BookMarkDB(QObject *parent = nullptr);
    ~BookMarkDB() Q_DECL_OVERRIDE;

    // DBFactory interface
public:
    void saveData() Q_DECL_OVERRIDE;
    void saveAsData(const QString &newPath) Q_DECL_OVERRIDE;
    void qSelectData() Q_DECL_OVERRIDE;

    QList<int> getBookMarkList() const;
    void setBookMarkList(const QList<int> &pBookMarkList);

    // DBFactory interface
private:
    void checkDatabase() Q_DECL_OVERRIDE;
    void clearInvalidRecord() Q_DECL_OVERRIDE;

    void insertBookMark(const QString &);
    void updateBookMark(const QString &);
    void deleteBookMark();

private:
    QString         m_strPageNumber = "PageNumber";
    QString         m_strTableName = "BookMarkTable";
    QList<int>      m_pBookMarkList;
};

#endif // DBMANAGER_H

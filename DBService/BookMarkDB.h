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

#include <QMap>

class BookMarkDB : public DBFactory
{
    Q_OBJECT
    Q_DISABLE_COPY(BookMarkDB)

public:
    explicit BookMarkDB(QObject *parent = nullptr);
    ~BookMarkDB() override;

    // DBFactory interface
public:
    void saveData(const QString &newPath) override;
    void qSelectData(const QString &) override;

    QList<int> getBookMarkList(const QString &) const;
    void setBookMarkList(const QString &, const QList<int> &pBookMarkList);

    // DBFactory interface
private:
    void checkDatabase() override;
    void clearInvalidRecord() override;

    void insertData(const QString &, const QString &);
    void updateData(const QString &, const QString &);
    void deleteData(const QString &);

private:
    QString         m_strPageNumber = "PageNumber";
    QString         m_strTableName = "BookMarkTable";
    QMap<QString, QList<int> >       m_pBookMarkMap;
};

#endif // DBMANAGER_H

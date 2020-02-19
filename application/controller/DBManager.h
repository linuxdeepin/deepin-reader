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
#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QMutex>

class QSqlDatabase;

class DBManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DBManager)

public:
    explicit DBManager(QObject *parent = nullptr);

protected:
    const QSqlDatabase getDatabase();

public:
    void setStrFilePath(const QString &strFilePath);

protected:
    bool hasFilePathDB(const QString &);

private:
    QList<int>      m_pBookMarkList;
    int             m_nDbType = -1;   //  原来是否有数据

    QString m_connectionName = "";

protected:
    QString m_strFilePath = "";
    QString m_strFileName = "";
    QMutex m_mutex;
};

#endif // DBMANAGER_H

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
#ifndef DBFACTORY_H
#define DBFACTORY_H

#include <QObject>
#include <QMutex>

class QSqlDatabase;

class DBFactory : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DBFactory)

public:
    explicit DBFactory(QObject *parent = nullptr);

public:
    void setStrFilePath(const QString &strFilePath);
    virtual void saveData(const QString &) = 0;
    virtual void qSelectData() = 0;

protected:
    virtual void checkDatabase() = 0;
    virtual void clearInvalidRecord() = 0;

protected:
    const QSqlDatabase getDatabase();
    bool hasFilePathDB(const QString &, const QString &);

protected:
    QString m_strFilePath = "";
    QString m_strFileName = "";
    QMutex m_mutex;
};

#endif // DBMANAGER_H

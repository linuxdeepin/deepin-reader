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
#include "DBFactory.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QStandardPaths>

const QString g_db_name = "deepinreader.db";

DBFactory::DBFactory(QObject *parent)
    : QObject(parent)
{

}

const QSqlDatabase DBFactory::getDatabase()
{
    QString m_connectionName = "default_connection";
    QMutexLocker mutex(&m_mutex);
    if (QSqlDatabase::contains(m_connectionName)) {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        return db;
    } else {
        //if database not open, open it.
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
        QString sDbPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        db.setDatabaseName(sDbPath + "/" + g_db_name);
        if (! db.open()) {
            qWarning() << "Open database error:" << db.lastError();
            return QSqlDatabase();
        } else {
            return db;
        }
    }
}

bool DBFactory::hasFilePathDB(const QString &strFilePath, const QString &sTabelName)
{
    const QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);
        QString sSql = QString("select count(*) from %1 where FilePath=?").arg(sTabelName);
        query.prepare(sSql);
        query.addBindValue(strFilePath);
        if (query.exec() && query.next()) {
            return query.value(0).toInt() == 1;
        } else {
            qWarning() << __FUNCTION__ << "   " << query.lastError();
        }
    }
    return false;
}

void DBFactory::setStrFilePath(const QString &strFilePath)
{
    m_strFilePath = strFilePath;

    int nLastPos = strFilePath.lastIndexOf('/');
    nLastPos++;
    m_strFileName = strFilePath.mid(nLastPos);
}


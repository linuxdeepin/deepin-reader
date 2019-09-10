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
#include "DBManager.h"
#include "application.h"
//#include "signalmanager.h"
//#include "utils/baseutils.h"
#include <QDebug>
#include <QDir>
#include <QMutex>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

namespace {

const QString DATABASE_PATH = QDir::homePath() + "/.local/share/deepin/deepin-reader/";
const QString DATABASE_NAME = "deepinreader.db";
//const QString EMPTY_HASH_STR = utils::base::hash(QString(" "));

}  // namespace

DBManager *DBManager::m_dbManager = nullptr;

DBManager *DBManager::instance()
{
    if (!m_dbManager) {
        m_dbManager = new DBManager();
    }

    return m_dbManager;
}

QString DBManager::getBookMarks()
{
    QString sData = "";
    const QSqlDatabase db = getDatabase();
    if (! db.isValid())
        return sData;

    QMutexLocker mutex(&m_mutex);
    QSqlQuery query( db );
    query.setForwardOnly(true);
    query.prepare( "SELECT PageNumber FROM BookMarkTable ORDER BY Time DESC where FilePath = '" + m_strFilePath + "'");
    if (! query.exec()) {
        qWarning() << "Get Data from BookMarkTable failed: " << query.lastError();
        mutex.unlock();
        return sData;
    }

    while (query.next()) {
        sData = query.value(0).toString();
    }
    mutex.unlock();

    return sData;
}

//  新增标签数据
void DBManager::insertBookMark(const QString &pageNumber)
{
    const QSqlDatabase db = getDatabase();

    QMutexLocker mutex(&m_mutex);
    // Insert into BookMarkTable
    QSqlQuery query( db );
    query.setForwardOnly(true);
    query.exec("BEGIN IMMEDIATE TRANSACTION");
    query.prepare( "REPLACE INTO BookMarkTable "
                   "(FilePath, FileName, PageNumber, Time) VALUES (?, ?, ?, ?)" );
    query.addBindValue(m_strFilePath);
    query.addBindValue(m_strFileName);
    query.addBindValue(pageNumber);

    QString nowTime = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
    query.addBindValue(nowTime);

    if (! query.execBatch()) {
        qWarning() << "Insert data into BookMarkTable failed: "
                   << query.lastError();
    }
    query.exec("COMMIT");
    mutex.unlock();
}

//  更新标签数据
void DBManager::updateBookMark(const QString &pageNumber)
{
    const QSqlDatabase db = getDatabase();

    QMutexLocker mutex(&m_mutex);
    // Insert into BookMarkTable
    QSqlQuery query( db );
    query.setForwardOnly(true);
    query.exec("BEGIN IMMEDIATE TRANSACTION");
    query.prepare( "REPLACE UPDATE BookMarkTable "
                   "set PageNumber= ? where FilePath = ? and m_strFileName = ?" );
    query.addBindValue(pageNumber);
    query.addBindValue(m_strFilePath);
    query.addBindValue(m_strFileName);

    if (! query.execBatch()) {
        qWarning() << "update BookMarkTable failed: "
                   << query.lastError();
    }
    query.exec("COMMIT");
    mutex.unlock();
}

DBManager::DBManager(QObject *parent)
    : QObject(parent)
    , m_connectionName("default_connection")
{
    checkDatabase();
}


const QSqlDatabase DBManager::getDatabase() const
{
    QMutexLocker mutex(&m_mutex);
    if ( QSqlDatabase::contains(m_connectionName) ) {
        QSqlDatabase db = QSqlDatabase::database(m_connectionName);
        mutex.unlock();
        return db;
    } else {
        //if database not open, open it.
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);//not dbConnection
        db.setDatabaseName(DATABASE_PATH + DATABASE_NAME);
        if (! db.open()) {
            qWarning() << "Open database error:" << db.lastError();
            mutex.unlock();
            return QSqlDatabase();
        } else {
            mutex.unlock();
            return db;
        }
    }
}

void DBManager::checkDatabase()
{
    QDir dd(DATABASE_PATH);
    if (! dd.exists()) {
        qDebug() << "create database paths";
        dd.mkpath(DATABASE_PATH);
        if (dd.exists())
            qDebug() << "create database succeed!";
        else
            qDebug() << "create database failed!";
    } else {
        qDebug() << "database is exist!";
    }
    const QSqlDatabase db = getDatabase();
    if (! db.isValid()) {
        return;
    }
    bool tableExist = false;
    QMutexLocker mutex(&m_mutex);
    QSqlQuery query( db );
    query.setForwardOnly(true);
    query.prepare( "SELECT name FROM sqlite_master "
                   "WHERE type=\"table\" AND name = \"BookMarkTable\"");
    if (query.exec() && query.first()) {
        tableExist = !query.value(0).toString().isEmpty();
    }

    //if tables not exist, create it.
    if ( !tableExist ) {
        QSqlQuery query(db);
        // ImageTable3
        //////////////////////////////////////////////////////////////
        //PathHash           | FilePath | FileName   | Dir  | Time  //
        //TEXT primari key   | TEXT     | TEXT       | TEXT | TEXT  //
        //////////////////////////////////////////////////////////////
        query.exec( QString("CREATE TABLE IF NOT EXISTS BookMarkTable ( "
                            "FilePath TEXT primary key, "
                            "FileName TEXT, "
                            "PageNumber TEXT, "
                            "Time TEXT )"));
    }

    mutex.unlock();
}

void DBManager::setStrFilePath(const QString &strFilePath)
{
    m_strFilePath = strFilePath;
}

void DBManager::setStrFileName(const QString &strFileName)
{
    m_strFileName = strFileName;
}

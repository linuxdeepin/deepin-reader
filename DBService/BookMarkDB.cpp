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

#include "BookMarkDB.h"

#include <QDebug>
#include <QFile>
#include <QVariant>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>

BookMarkDB::BookMarkDB(QObject *parent)
    : DBFactory(parent)
{
    checkDatabase();
    clearInvalidRecord();
}

BookMarkDB::~BookMarkDB()
{

}

void BookMarkDB::saveData(const QString &newpath)
{
    if (m_pBookMarkList.size() == 0) {
        deleteData();
    } else {
        QString sPage = "";
        foreach (int i, m_pBookMarkList) {
            sPage += QString::number(i) + ",";
        }

        if (!hasFilePathDB(newpath, m_strTableName)) {
            insertData(newpath, sPage);
        } else {
            updateData(newpath, sPage);
        }
    }
}

void BookMarkDB::qSelectData()
{
    m_pBookMarkList.clear();
    const QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);

        QString sSql = QString("SELECT %1 FROM %2 where FilePath = ? and FileName = ?;").arg(m_strPageNumber).arg(m_strTableName);
        query.prepare(sSql);
        query.addBindValue(m_strFilePath);
        query.addBindValue(m_strFileName);
        if (query.exec() && query.next()) {
            QString sData = query.value(0).toString();      //  结果

            QStringList sPageList = sData.split(",", QString::SkipEmptyParts);
            foreach (QString s, sPageList) {
                int nPage = s.toInt();
                m_pBookMarkList.append(nPage);
            }

            qSort(m_pBookMarkList.begin(), m_pBookMarkList.end());
        } else {
            qWarning() << __func__ << " error:  " << query.lastError();
        }
    }
}

//  新增标签数据
void BookMarkDB::insertData(const QString &strFilePath, const QString &pageNumber)
{
    QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);

        QString sSql = QString("INSERT INTO %1 (FilePath, FileName, %2) VALUES (?, ?, ?);").arg(m_strTableName).arg(m_strPageNumber);
        query.prepare(sSql);

        query.addBindValue(strFilePath);
        query.addBindValue(m_strFileName);
        query.addBindValue(pageNumber);

        if (query.exec()) {
            db.commit();
        } else {
            db.rollback();
        }
    }
}

//  更新标签数据
void BookMarkDB::updateData(const QString &strFilePath, const QString &pageNumber)
{
    QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);

        QString sSql = QString("UPDATE %1 set %2 = ? where FilePath = ? and FileName = ?;").arg(m_strTableName).arg(m_strPageNumber);
        query.prepare(sSql);

        query.addBindValue(pageNumber);
        query.addBindValue(strFilePath);
        query.addBindValue(m_strFileName);

        if (query.exec()) {
            db.commit();
        } else {
            db.rollback();
            qWarning() << __FUNCTION__ << "  error:     " << query.lastError();
        }
    }
}

void BookMarkDB::deleteData()
{
    QSqlDatabase db = getDatabase();
    if (db.isValid()) {

        QMutexLocker mutex(&m_mutex);
        // delete into BookMarkTable
        QSqlQuery query(db);

        QString sSql = QString("DELETE FROM %1 where FilePath = ? and FileName = ?;").arg(m_strTableName);
        query.prepare(sSql);

        query.addBindValue(m_strFilePath);
        query.addBindValue(m_strFileName);

        if (query.exec()) {
            db.commit();
        } else {
            db.rollback();
            qDebug() << __FUNCTION__ << "      error:      " << query.lastError();
        }
    }
}

/**
 * @brief BookMarkDB::clearInvalidRecord
 * 删除不存在文件(绝对路径)
 */
void BookMarkDB::clearInvalidRecord()
{
    const QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);
        QString sSql = QString("select FilePath from %1").arg(m_strTableName);
        query.prepare(sSql);
        if (query.exec()) {
            QString strsql;
            while (query.next()) {
                QString strpath = query.value(0).toString();
                if (!QFile::exists(strpath)) {
                    QString sSql = QString("delete from %1 where FilePath='%2';").arg(m_strTableName).arg(strpath);
                    strsql.append(sSql);
                }
            }
            query.clear();
            if (!strsql.isEmpty()) {
                query.prepare(strsql);
                if (!query.exec())
                    qDebug() << __LINE__ << "   " << __FUNCTION__ << "   " << query.lastError();
            }
        }
    }
}

void BookMarkDB::checkDatabase()
{
    const QSqlDatabase db = getDatabase();
    if (! db.isValid()) {
        return;
    }
    bool tableExist = false;
    QMutexLocker mutex(&m_mutex);
    QSqlQuery query(db);

    query.prepare(QString("SELECT name FROM sqlite_master WHERE type=\"table\" AND name = '%1'").arg(m_strTableName));
    if (query.exec() && query.first()) {
        tableExist = !query.value(0).toString().isEmpty();
    }

    //if tables not exist, create it.
    if (!tableExist) {
        //////////////////////////////////////////////////////////////
        //PathHash           | FilePath | FileName   | Dir  | Time  //
        //TEXT primari key   | TEXT     | TEXT       | TEXT | TEXT  //
        //////////////////////////////////////////////////////////////

        QString sql = QString("CREATE TABLE IF NOT EXISTS %1 ( "
                              "FilePath TEXT primary key, "
                              "FileName TEXT, "
                              "%2 TEXT, "
                              "Time TEXT )").arg(m_strTableName).arg(m_strPageNumber);
        query.exec(sql);
    }
}

void BookMarkDB::setBookMarkList(const QList<int> &pBookMarkList)
{
    m_pBookMarkList = pBookMarkList;
}

QList<int> BookMarkDB::getBookMarkList() const
{
    return m_pBookMarkList;
}

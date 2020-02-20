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
#include "HistroyDB.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QFile>
#include <QDebug>

HistroyDB::HistroyDB(QObject *parent)
    : DBFactory(parent)
{
    checkDatabase();
    clearInvalidRecord();
}

void HistroyDB::saveData(const QString &newPath)
{
    if (!hasFilePathDB(newPath, m_strTableName)) {
        insertData(newPath);
    } else {
        updateData(newPath);
    }
}

void HistroyDB::qSelectData()
{
    const QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);
        query.setForwardOnly(true);
        QString sql = QString("SELECT * FROM %1 where FilePath = '%2'").arg(m_strTableName).arg(m_strFilePath);

        if (query.exec(sql) && query.next()) {
            m_pDataObj.insert("scale", query.value(1).toInt());          // 缩放
            m_pDataObj.insert("doubleShow", query.value(2).toInt());     // 是否是双页
            m_pDataObj.insert("fit", query.value(3).toInt());            // 自适应宽/高
            m_pDataObj.insert("rotate", query.value(4).toInt());         // 文档旋转角度(0~360)
            m_pDataObj.insert("leftState", query.value(5).toInt());      //  左侧列表窗口是否显示
            m_pDataObj.insert("listIndex", query.value(6).toInt());      // 在哪个列表
            m_pDataObj.insert("curPage", query.value(7).toInt());        // 文档当前页
        }
    }
}

QJsonObject HistroyDB::getHistroyData() const
{
    return m_pDataObj;
}

//  新的数据
void HistroyDB::setHistroyData(const QString &sKey, const int &iValue)
{
    m_pNewDataObj[sKey] = iValue;
}

void HistroyDB::checkDatabase()
{
    const QSqlDatabase db = getDatabase();
    if (! db.isValid()) {
        return;
    }
    bool tableExist = false;
    QMutexLocker mutex(&m_mutex);
    QSqlQuery query(db);

    query.prepare("SELECT name FROM sqlite_master WHERE type=\"table\" AND name = \"FilesTable\"");
    if (query.exec() && query.first()) {
        tableExist = !query.value(0).toString().isEmpty();
    }

    //if FilesTable not exist, create it.
    if (!tableExist) {
        QSqlQuery query(db);
        query.exec(QString("CREATE TABLE IF NOT EXISTS FilesTable ( "
                           "FilePath TEXT primary key, "
                           "FileScale TEXT, "
                           "FileDoubPage TEXT, "
                           "FileFit TEXT, "
                           "FileRotate TEXT, "
                           "FileShowLeft TEXT, "
                           "ListIndex TEXT, "
                           "CurPage TEXT )"));
    }
}

void HistroyDB::clearInvalidRecord()
{
    const QSqlDatabase db = getDatabase();
    QMutexLocker mutex(&m_mutex);
    QSqlQuery query(db);
    query.prepare("select FilePath from FilesTable");
    if (query.exec()) {
        QString strsql;
        while (query.next()) {
            QString strpath = query.value(0).toString();
            if (!QFile::exists(strpath)) {
                strsql.append(QString("delete from %1 where FilePath='%2';").arg(m_strTableName).arg(strpath));
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

void HistroyDB::insertData(const QString &sPath)
{
    QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);

        QString sSql = QString("INSERT INTO %1 (FilePath, FileScale, FileDoubPage, FileFit, FileRotate, FileShowLeft, ListIndex, CurPage) "
                               "VALUES (?, ?, ?, ?, ?, ?, ?, ?)").arg(m_strTableName);
        query.prepare(sSql);

        query.addBindValue(sPath);
        query.addBindValue(GetKeyValue("scale"));
        query.addBindValue(GetKeyValue("doubleShow"));
        query.addBindValue(GetKeyValue("fit"));
        query.addBindValue(GetKeyValue("rotate"));
        query.addBindValue(GetKeyValue("leftState"));
        query.addBindValue(GetKeyValue("listIndex"));
        query.addBindValue(GetKeyValue("curPage"));

        if (query.exec()) {
            db.commit();
        } else {
            db.rollback();
            qWarning() << __func__ << " error:  " << query.lastError();
        }
    }
}

void HistroyDB::updateData(const QString &sPath)
{
    QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);

        QString sSql = QString("UPDATE %1 set "
                               "FileScale = ?, FileDoubPage = ?, "
                               "FileFit = ?, FileRotate = ?, "
                               "FileShowLeft = ?, ListIndex = ?, "
                               "CurPage = ? where FilePath = ?").arg(m_strTableName);
        query.prepare(sSql);

        query.addBindValue(GetKeyValue("scale"));
        query.addBindValue(GetKeyValue("doubleShow"));
        query.addBindValue(GetKeyValue("fit"));
        query.addBindValue(GetKeyValue("rotate"));
        query.addBindValue(GetKeyValue("leftState"));
        query.addBindValue(GetKeyValue("listIndex"));
        query.addBindValue(GetKeyValue("curPage"));
        query.addBindValue(sPath);

        if (query.exec()) {
            db.commit();
        } else {
            db.rollback();
            qWarning() << __func__ << " error:  " << query.lastError();
        }
    }
}

void HistroyDB::deleteData()
{

}

int HistroyDB::GetKeyValue(const QString &sKey)
{
    if (m_pNewDataObj[sKey].isNull()) {
        return m_pDataObj[sKey].toInt();
    }

    return m_pNewDataObj[sKey].toInt();
}

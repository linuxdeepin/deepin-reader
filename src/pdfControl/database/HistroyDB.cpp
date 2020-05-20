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
}

void HistroyDB::saveData(const QString &newPath)
{
    if (!hasFilePathDB(newPath, m_strTableName)) {
        insertData(newPath);
    } else {
        updateData(newPath);
    }
}

void HistroyDB::qSelectData(const QString &sPath)
{
    const QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);

        query.setForwardOnly(true);
        QString sql = QString("SELECT * FROM %1 where FilePath = '%2'").arg(m_strTableName).arg(sPath);

        if (query.exec(sql)) {
            if (query.next()) {
                FileDataModel dataObj;
                dataObj.setOper(Scale, query.value("FileScale"));           // 缩放
                dataObj.setOper(DoubleShow, query.value("FileDoubPage"));   // 是否是双页
                dataObj.setOper(Fit, query.value("FileFit"));               // 自适应宽/高
                dataObj.setOper(Rotate, query.value("FileRotate"));         // 文档旋转角度(0~360)
                dataObj.setOper(Thumbnail, query.value("FileShowLeft"));    //  左侧列表窗口是否显示
                dataObj.setOper(LeftIndex, query.value("ListIndex"));       // 在哪个列表
                dataObj.setOper(CurIndex, query.value("CurPage"));           // 文档当前页

                m_pDataMapObj.insert(sPath, dataObj);
            } else {
                qWarning() << __func__ << " no data ";
            }
        }
    }
}

FileDataModel HistroyDB::getHistroyData(const QString &sPath) const
{
    if (m_pDataMapObj.contains(sPath)) {
        return m_pDataMapObj[sPath];
    }
    return  FileDataModel();
}

//  新的数据
void HistroyDB::setHistroyData(const QString &sPath, const int &iKey, const QVariant &iValue)
{
    if (m_pNewDataMapObj.contains(sPath)) {
        m_pNewDataMapObj[sPath].setOper(iKey, iValue);
    } else {
        FileDataModel dataObj;
        dataObj.setOper(iKey, iValue);
        m_pNewDataMapObj.insert(sPath, dataObj);
    }
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

        qDebug() << __LINE__ << "   " << __FUNCTION__ << "   " << query.lastError();
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
        query.addBindValue(GetKeyValue(sPath, Scale));
        query.addBindValue(GetKeyValue(sPath, DoubleShow));
        query.addBindValue(GetKeyValue(sPath, Fit));
        query.addBindValue(GetKeyValue(sPath, Rotate));
        query.addBindValue(GetKeyValue(sPath, Thumbnail));
        query.addBindValue(GetKeyValue(sPath, LeftIndex));
        query.addBindValue(GetKeyValue(sPath, CurIndex));

        if (!query.exec()) {
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

        query.addBindValue(GetKeyValue(sPath, Scale));
        query.addBindValue(GetKeyValue(sPath, DoubleShow));
        query.addBindValue(GetKeyValue(sPath, Fit));
        query.addBindValue(GetKeyValue(sPath, Rotate));
        query.addBindValue(GetKeyValue(sPath, Thumbnail));
        query.addBindValue(GetKeyValue(sPath, LeftIndex));
        query.addBindValue(GetKeyValue(sPath, CurIndex));
        query.addBindValue(sPath);

        if (!query.exec()) {
            qWarning() << __func__ << " error:  " << query.lastError();
        }
    }
}

QString HistroyDB::GetKeyValue(const QString &sPath, const int &iKey)
{
    if (m_pNewDataMapObj.contains(sPath)) {

        QVariant nTemp = m_pNewDataMapObj[sPath].getOper(iKey);

        if (-1 == nTemp.toInt())

            return m_pDataMapObj[sPath].getOper(iKey).toString();

        return nTemp.toString();
    }

    return m_pDataMapObj[sPath].getOper(iKey).toString();
}

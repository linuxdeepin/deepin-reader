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
//    clearInvalidRecord();
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
                dataObj.qSetData(Scale, query.value("FileScale").toDouble());          // 缩放
                dataObj.qSetData(DoubleShow, query.value("FileDoubPage").toDouble());     // 是否是双页
                dataObj.qSetData(Fit, query.value("FileFit").toDouble());            // 自适应宽/高
                dataObj.qSetData(Rotate, query.value("FileRotate").toDouble());         // 文档旋转角度(0~360)
                dataObj.qSetData(Thumbnail, query.value("FileShowLeft").toDouble());      //  左侧列表窗口是否显示
                dataObj.qSetData(LeftIndex, query.value("ListIndex").toDouble());      // 在哪个列表
                dataObj.qSetData(CurPage, query.value("CurPage").toDouble());         // 文档当前页

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
void HistroyDB::setHistroyData(const QString &sPath, const int &iKey, const double &iValue)
{
    if (m_pNewDataMapObj.contains(sPath)) {
        m_pNewDataMapObj[sPath].qSetData(iKey, iValue);
    } else {
        FileDataModel dataObj;
        dataObj.qSetData(iKey, iValue);
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
        query.addBindValue(GetKeyValue(sPath, CurPage));

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

        double scale = 1.0;
        scale = GetKeyValue(sPath, Scale);
        query.addBindValue(GetKeyValue(sPath, Scale));
        query.addBindValue(GetKeyValue(sPath, DoubleShow));
        query.addBindValue(GetKeyValue(sPath, Fit));
        query.addBindValue(GetKeyValue(sPath, Rotate));
        query.addBindValue(GetKeyValue(sPath, Thumbnail));
        query.addBindValue(GetKeyValue(sPath, LeftIndex));
        query.addBindValue(GetKeyValue(sPath, CurPage));
        query.addBindValue(sPath);

        if (!query.exec()) {
            qWarning() << __func__ << " error:  " << query.lastError();
        }
    }
}

double HistroyDB::GetKeyValue(const QString &sPath, const int &iKey)
{
    if (m_pNewDataMapObj.contains(sPath)) {
        double nTemp = m_pNewDataMapObj[sPath].qGetData(iKey);
        if (-1 == static_cast<int>(nTemp))
            return m_pDataMapObj[sPath].qGetData(iKey);
        return nTemp;
    }

    return m_pDataMapObj[sPath].qGetData(iKey);
}

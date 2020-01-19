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

#include <QSqlQuery>
#include <QSqlError>
#include <QFile>

HistroyDB::HistroyDB(QObject *parent)
    : DBManager(parent)
{
    checkDatabases();
    clearInvalidRecords();
}

void HistroyDB::checkDatabases()
{
    const QSqlDatabase db = getDatabase();
    if (! db.isValid()) {
        return;
    }
    bool tableExist = false;
    QMutexLocker mutex(&m_mutex);
    QSqlQuery query(db);
    query.setForwardOnly(true);

    query.prepare("SELECT name FROM sqlite_master "
                  "WHERE type=\"table\" AND name = \"FilesTable\"");
    if (query.exec() && query.first()) {
        tableExist = !query.value(0).toString().isEmpty();
    }

    //if FilesTable not exist, create it.
    if (!tableExist) {
        QSqlQuery query(db);
        // FileFontTable  >> FilesTable
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //FilePath           | FileScale | FileDoubPage | FileFit  | FileRotate | FileShowLeft | ListIndex | CurPage //
        //TEXT primari key   | TEXT      | TEXT         | TEXT     | TEXT       | TEXT         | TEXT      | TEXT    //
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

void HistroyDB::clearInvalidRecords()
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
                strsql.append(QString("delete from FilesTable where FilePath='%1';").arg(strpath));
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

void HistroyDB::insertFileFontMsg(const st_fileHistoryMsg &msg, const QString filePath)
{
    QString t_strFilePath = "";
    (filePath != "") ? (t_strFilePath = filePath) : (t_strFilePath = m_strFilePath);
    if (hasFilePathDB(t_strFilePath)) {
        updateFileFontMsg(msg, t_strFilePath);
    } else {
        const QSqlDatabase db = getDatabase();

        QMutexLocker mutex(&m_mutex);
        // Insert into FilesTable
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.exec("START TRANSACTION");//开始事务。使用BEGIN也可以
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        //FilePath           | FileScale | FileDoubPage | FileFit  | FileRotate | FileShowLeft | ListIndex | CurPage //
        //TEXT primari key   | TEXT      | TEXT         | TEXT     | TEXT       | TEXT         | TEXT      | TEXT    //
        ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
        query.prepare("INSERT INTO FilesTable "
                      "(FilePath, FileScale, FileDoubPage, FileFit, FileRotate, FileShowLeft, ListIndex, CurPage) VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        if (t_strFilePath != "") {
            query.addBindValue(t_strFilePath);
        } else {
            query.addBindValue(m_strFilePath);
        }

        query.addBindValue(msg.m_strScale);//scale
        query.addBindValue(msg.m_strDoubPage);//doubPage
        query.addBindValue(msg.m_strFit);//fit
        query.addBindValue(msg.m_strRotate);//rotate
        query.addBindValue(msg.m_strShowLeft);//ShowLeft
        query.addBindValue(msg.m_strListIndex);//ListIndex
        query.addBindValue(msg.m_strCurPage);//CurPage

        if (query.exec()) {
            query.exec("COMMIT");
        } else {
            query.exec("ROLLBACK");//回滚
        }
    }
}

void HistroyDB::updateFileFontMsg(const st_fileHistoryMsg &msg, const QString filePath)
{
    const QSqlDatabase db = getDatabase();

    QMutexLocker mutex(&m_mutex);
    // update FilesTable
    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.exec("START TRANSACTION");//开始事务。使用BEGIN也可以
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //FilePath           | FileScale | FileDoubPage | FileFit  | FileRotate | FileShowLeft | ListIndex | CurPage //
    //TEXT primari key   | TEXT      | TEXT         | TEXT     | TEXT       | TEXT         | TEXT      | TEXT    //
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    query.prepare("UPDATE FilesTable "
                  "set FileScale = ?, FileDoubPage = ?, FileFit = ?, FileRotate = ?, FileShowLeft = ?, ListIndex = ?, CurPage = ? where FilePath = ?");

    query.addBindValue(msg.m_strScale);
    query.addBindValue(msg.m_strDoubPage);
    query.addBindValue(msg.m_strFit);
    query.addBindValue(msg.m_strRotate);
    query.addBindValue(msg.m_strShowLeft);
    query.addBindValue(msg.m_strListIndex);
    query.addBindValue(msg.m_strCurPage);
    if (filePath != "") {
        query.addBindValue(filePath);
    } else {
        query.addBindValue(m_strFilePath);
    }

    if (query.exec()) {
        query.exec("COMMIT");
    }
}
/**
 * @brief HistroyDB::saveFileFontMsg
 *         废弃
 * @param msg
 * @param filePath
 */
void HistroyDB::saveFileFontMsg(const st_fileHistoryMsg &, const QString)
{

}

void HistroyDB::saveAsFileFontMsg(const st_fileHistoryMsg &msg, const QString filePath)
{
    insertFileFontMsg(msg, filePath);
}

bool HistroyDB::getFileFontMsg(st_fileHistoryMsg &msg, const QString filePath)
{
    const QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);
        query.setForwardOnly(true);
        QString sql = QString("SELECT FileScale, FileDoubPage, FileFit, FileRotate, FileShowLeft, ListIndex, CurPage FROM FilesTable where FilePath = '%1'").arg(filePath);

        if (query.exec(sql)) {
            while (query.next()) {
                msg.m_strScale = query.value(0).toString();       // 缩放
                msg.m_strDoubPage = query.value(1).toString();    // 是否是双页
                msg.m_strFit = query.value(2).toString();         // 自适应宽/高
                msg.m_strRotate = query.value(3).toString();      // 文档旋转角度(0~360)
                msg.m_strShowLeft = query.value(4).toString();    //  左侧列表窗口是否显示
                msg.m_strListIndex = query.value(5).toString();   // 在哪个列表
                msg.m_strCurPage = query.value(6).toString();     // 文档当前页
                return false;
            }
        } else {
            return true;
        }
    }
    return true;
}

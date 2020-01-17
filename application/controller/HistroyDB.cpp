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

HistroyDB::HistroyDB(DBManager *parent)
    : DBManager(parent)
{
}

/**
 * @brief HistroyDB::insertFileFontMsg
 * 插入一条新数据
 */
void HistroyDB::insertFileFontMsg(const QString &scale, const QString &doubPage, const QString &fit, const QString &rotate, const QString strFilePath)
{
    QString t_strFilePath = "";
    (strFilePath != "") ? (t_strFilePath = strFilePath) : (t_strFilePath = m_strFilePath);
    if (hasFilePathDB(t_strFilePath)) {
        updateFileFontMsg(scale, doubPage, fit, rotate, t_strFilePath);
    } else {
        const QSqlDatabase db = getDatabase();

        QMutexLocker mutex(&m_mutex);
        // Insert into FileFontTable
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.exec("START TRANSACTION");//开始事务。使用BEGIN也可以
        query.prepare("INSERT INTO FileFontTable "
                      "(FilePath, FileScale, FileDoubPage, FileFit, FileRotate) VALUES (?, ?, ?, ?, ?)");
        if (t_strFilePath != "") {
            query.addBindValue(t_strFilePath);
        } else {
            query.addBindValue(m_strFilePath);
        }

        query.addBindValue(scale);
        query.addBindValue(doubPage);
        query.addBindValue(fit);
        query.addBindValue(rotate);

        if (query.exec()) {
            query.exec("COMMIT");
        } else {
            qDebug() << " insert FileFontTable error:" << query.lastError();
            query.exec("ROLLBACK");//回滚
        }
        mutex.unlock();
    }
}

/**
 * @brief HistroyDB::updateFileFontMsg
 * 更新已有数据库
 */
void HistroyDB::updateFileFontMsg(const QString &scale, const QString &doubPage, const QString &fit, const QString &rotate, const QString strFilePath)
{
    const QSqlDatabase db = getDatabase();

    QMutexLocker mutex(&m_mutex);
    // update FileFontTable
    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.exec("START TRANSACTION");//开始事务。使用BEGIN也可以
    query.prepare("UPDATE FileFontTable "
                  "set FileScale = ?, FileDoubPage = ?, FileFit = ?, FileRotate = ? where FilePath = ?");

    query.addBindValue(scale);
    query.addBindValue(doubPage);
    query.addBindValue(fit);
    query.addBindValue(rotate);
    if (strFilePath != "") {
        query.addBindValue(strFilePath);
    } else {
        query.addBindValue(m_strFilePath);
    }

    if (query.exec()) {
        query.exec("COMMIT");
    }
}

/**
 * @brief HistroyDB::deleteFileFontMsg
 * 删除无用数据
 */
void HistroyDB::deleteFileFontMsg()
{
    const QSqlDatabase db = getDatabase();
    QMutexLocker mutex(&m_mutex);
    QSqlQuery query(db);
    query.prepare("select FilePath from FileFontTable");
    if (query.exec()) {
        QString strsql;
        while (query.next()) {
            QString strpath = query.value(0).toString();
            if (!QFile::exists(strpath)) {
                strsql.append(QString("delete from FileFontTable where FilePath='%1';").arg(strpath));
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

/**
 * @brief HistroyDB::saveFileFontMsg
 * 保存文件字号菜单数据
 */
void HistroyDB::saveFileFontMsg()
{

}

/**
 * @brief HistroyDB::saveAsFileFontMsg
 * 另存文件字号菜单数据
 */
bool HistroyDB::saveAsFileFontMsg(const QString &scale, const QString &doubPage, const QString &fit, const QString &rotate, const QString newpath)
{
    bool bsuccess = false;
    const QSqlDatabase db = getDatabase();
    QMutexLocker mutex(&m_mutex);
    QSqlQuery query(db);
    query.setForwardOnly(true);
    query.prepare("select count(*) from FileFontTable where FilePath=?");
    query.addBindValue(newpath);
    if (query.exec()) {
        if (query.next() && query.value(0).toInt() <= 0) {

            query.clear();
            query.exec("START TRANSACTION");
            query.prepare("INSERT INTO FileFontTable "
                          "(FilePath, FileScale, FileDoubPage, FileFit, FileRotate) VALUES (?, ?, ?, ?)");

            query.addBindValue(newpath);
            query.addBindValue(scale);
            query.addBindValue(doubPage);
            query.addBindValue(fit);
            query.addBindValue(rotate);

            if (query.exec()) {
                query.exec("COMMIT");
                bsuccess = true;
            }
        }
    }
    return  bsuccess;
}

/**
 * @brief HistroyDB::getFileFontMsg
 * 获取文件对应的字号菜单数据
 */
void HistroyDB::getFileFontMsg(QString &scale, QString &doubPage, QString &fit, QString &rotate, const QString &filePath)
{
    const QSqlDatabase db = getDatabase();
    if (db.isValid()) {
        QMutexLocker mutex(&m_mutex);
        QSqlQuery query(db);
        query.setForwardOnly(true);
        query.prepare("SELECT FileScale, FileDoubPage, FileFit, FileRotate FROM FileFontTable where FilePath = ?");
        query.addBindValue(filePath);

        if (query.exec()) {
            while (query.next()) {
                scale = query.value(0).toString();      //  缩放
                doubPage = query.value(1).toString();   //  是否是双页
                fit = query.value(2).toString();        //  自适应宽/高
                rotate = query.value(3).toString();     //  文档旋转角度
            }
        }
        mutex.unlock();
    }
}

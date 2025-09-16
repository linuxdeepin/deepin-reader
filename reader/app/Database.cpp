// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Database.h"
#include "DocSheet.h"
#include "Global.h"
#include "DocSheet.h"
#include "ddlog.h"

#include <QApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QSqlError>
#include <QSqlQuery>

Transaction::Transaction(QSqlDatabase &database)
    : m_committed(false), m_database(database)
{
    qCDebug(appLog) << "Starting database transaction";
    if (!m_database.transaction()) {
        qCWarning(appLog) << "Failed to start transaction:" << m_database.lastError();
    }
}

Transaction::~Transaction()
{
    // qCDebug(appLog) << "Transaction::~Transaction() start";
    if (!m_committed) {
        if (!m_database.rollback()) {
            qCInfo(appLog) << m_database.lastError();
        }
    }
    // qCDebug(appLog) << "Transaction::~Transaction() end";
}

void Transaction::commit()
{
    qCDebug(appLog) << "Committing transaction";
    m_committed = m_database.commit();
    if (!m_committed) {
        qCWarning(appLog) << "Failed to commit transaction:" << m_database.lastError();
    }
    // qCDebug(appLog) << "Transaction::commit() end";
}


Database *Database::s_instance = nullptr;

Database *Database::instance()
{
    // qCDebug(appLog) << "Database::instance() start";
    if (s_instance == nullptr) {
        // qCDebug(appLog) << "Creating new Database instance";
        s_instance = new Database(qApp);
    }

    // qCDebug(appLog) << "Returning Database instance";
    return s_instance;
}

Database::~Database()
{
    // qCDebug(appLog) << __FUNCTION__ << " start";
    m_database.close();
    s_instance = nullptr;
    // qCDebug(appLog) << "Database::~Database() end";
}

bool Database::prepareOperation()
{
    // qCDebug(appLog) << "prepareOperation start";
    Transaction transaction(m_database);

    qCDebug(appLog) << "Preparing operation table";
    QSqlQuery query(m_database);
    if (!query.exec("CREATE TABLE operation "
                    "(filePath TEXT primary key"
                    ",layoutMode INTEGER"
                    ",mouseShape INTEGER"
                    ",scaleMode INTEGER"
                    ",rotation INTEGER"
                    ",scaleFactor REAL"
                    ",sidebarVisible INTEGER"
                    ",sidebarIndex INTEGER"
                    ",currentPage INTEGER)")) {
        qCInfo(appLog) << query.lastError();
        return false;
    }

    if (!query.isActive()) {
        qCInfo(appLog) << query.lastError();
        return false;
    }

    transaction.commit();
    // qCDebug(appLog) << "prepareOperation end";
    return true;
}

bool Database::readOperation(DocSheet *sheet)
{
    // qCDebug(appLog) << "readOperation start";
    if(!sheet) {
        // qCDebug(appLog) << __FUNCTION__ << " end";
        return false;
    }

    // qCDebug(appLog) << "Reading operation for file:" << sheet->filePath();
    QSqlQuery query(m_database);
    query.prepare(" select * from operation where filePath = :filePath");
    query.bindValue(":filePath", sheet->filePath());
    if (!query.exec()) {
        qCWarning(appLog) << "Failed to read operation:" << query.lastError();
    }
    if (query.next()) {
        // qCDebug(appLog) << "Reading operation for file:" << sheet->filePath();
        sheet->m_operation.layoutMode = static_cast<Dr::LayoutMode>(query.value("layoutMode").toInt());
        sheet->m_operation.mouseShape = static_cast<Dr::MouseShape>(query.value("mouseShape").toInt());
        sheet->m_operation.scaleMode = static_cast<Dr::ScaleMode>(query.value("scaleMode").toInt());
        sheet->m_operation.rotation = static_cast<Dr::Rotation>(query.value("rotation").toInt());
        sheet->m_operation.scaleFactor = qBound(0.1, query.value("scaleFactor").toDouble(), 5.0);
        sheet->m_operation.sidebarVisible = query.value("sidebarVisible").toInt();
        sheet->m_operation.sidebarIndex = query.value("sidebarIndex").toInt();
        sheet->m_operation.currentPage = query.value("currentPage").toInt();
        return true;
    }
    // qCDebug(appLog) << "readOperation end";
    return false;
}

bool Database::saveOperation(DocSheet *sheet)
{
    // qCDebug(appLog) << "saveOperation start";
    if(!sheet) {
        // qCDebug(appLog) << "saveOperation end";
        return false;
    }

    QSqlQuery query(m_database);
    query.prepare(" replace into "
                  " operation(filePath,layoutMode,mouseShape,scaleMode,rotation,scaleFactor,sidebarVisible,sidebarIndex,currentPage)"
                  " VALUES(:filePath,:layoutMode,:mouseShape,:scaleMode,:rotation,:scaleFactor,:sidebarVisible,:sidebarIndex,:currentPage)");
    query.bindValue(":filePath", sheet->filePath());
    query.bindValue(":layoutMode", sheet->m_operation.layoutMode);
    query.bindValue(":mouseShape", sheet->m_operation.mouseShape);
    query.bindValue(":scaleMode", sheet->m_operation.scaleMode);
    query.bindValue(":rotation", sheet->m_operation.rotation);
    query.bindValue(":scaleFactor", sheet->m_operation.scaleFactor);
    query.bindValue(":sidebarVisible", sheet->m_operation.sidebarVisible);
    query.bindValue(":sidebarIndex", sheet->m_operation.sidebarIndex);
    query.bindValue(":currentPage", sheet->m_operation.currentPage);

    if(!query.exec()){
        qCInfo(appLog) << query.lastError().text();
        return false;
    }
    // qCDebug(appLog) << "saveOperation end";
    return true;
}

bool Database::prepareBookmark()
{
    qCDebug(appLog) << "Preparing bookmark table";
    Transaction transaction(m_database);

    QSqlQuery query(m_database);
    if (!query.exec("CREATE TABLE bookmark(filePath TEXT,bookmarkIndex INTEGER)")) {
        qCWarning(appLog) << "Failed to create bookmark table:" << query.lastError();
        return false;
    }

    if (!query.isActive()) {
        qCInfo(appLog) << query.lastError();
        return false;
    }

    transaction.commit();
    qCDebug(appLog) << "prepareBookmark end, return true";
    return true;
}

bool Database::readBookmarks(const QString &filePath, QSet<int> &bookmarks)
{
    qCDebug(appLog) << "readBookmarks start";
    if (m_database.isOpen()) {
        QSqlQuery query(m_database);

        if (!query.prepare(" select * from bookmark where filePath = :filePath")) {
            qCInfo(appLog) << query.lastError();
            return false;
        }
        query.bindValue(":filePath", filePath);

        if (!query.exec()) {
            qCInfo(appLog) << query.lastError().text();
            return false;
        }

        while (query.next()) {
            bookmarks.insert(query.value("bookmarkIndex").toInt());
        }

        qCDebug(appLog) << "readBookmarks end, return true";
        return true;
    }

    qCDebug(appLog) << "readBookmarks end, return false";
    return false;
}

bool Database::saveBookmarks(const QString &filePath, const QSet<int> bookmarks)
{
    qCDebug(appLog) << "saveBookmarks start";
    if (m_database.isOpen()) {
        QSqlQuery query(m_database);

        Transaction transaction(m_database);

        if (!query.prepare("delete from bookmark where filePath = :filePath")) {
            qCInfo(appLog) << query.lastError();
            return false;
        }

        query.bindValue(":filePath", filePath);

        if (!query.exec()) {
            qCInfo(appLog) << query.lastError().text();
            return false;
        }

        foreach (int index, bookmarks) {
            if (!query.prepare(" insert into "
                               " bookmark(filePath,bookmarkIndex)"
                               " VALUES(:filePath,:bookmarkIndex)")) {
                qCInfo(appLog) << query.lastError();
                return false;
            }

            query.bindValue(":filePath", filePath);

            query.bindValue(":bookmarkIndex", index);

            if (!query.exec()) {
                qCInfo(appLog) << query.lastError().text();
                return false;
            }
        }

        transaction.commit();

        qCDebug(appLog) << "saveBookmarks end, return true";
        return true;
    }

    qCDebug(appLog) << "saveBookmarks end, return false";
    return false;
}

Database::Database(QObject *parent) : QObject(parent)
{
    qCDebug(appLog) << "Initializing database connection";
    const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);

    qCDebug(appLog) << "Database path:" << path;
    QDir().mkpath(path);

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName(QDir(path).filePath("user.db"));
    qCDebug(appLog) << "Opening database:" << m_database.databaseName();
    if (!m_database.open()) {
        qCritical() << "Failed to open database:" << m_database.lastError();
    }

    if (m_database.isOpen()) {
        qCDebug(appLog) << "Setting database optimization parameters";
        {
            QSqlQuery query(m_database);
            if (!query.exec("PRAGMA synchronous = OFF")) {
                qCWarning(appLog) << "Failed to set synchronous mode:" << query.lastError();
            }
            if (!query.exec("PRAGMA journal_mode = MEMORY")) {
                qCWarning(appLog) << "Failed to set journal mode:" << query.lastError();
            }
        }

        const QStringList tables = m_database.tables();

        if (!tables.contains("operation")) {
            prepareOperation();
        }

        if (!tables.contains("bookmark")) {
            prepareBookmark();
        }
    } else {
        qCInfo(appLog) << m_database.lastError();
    }
    qCDebug(appLog) << "Database::Database() end";
}

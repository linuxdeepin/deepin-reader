// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
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
#include <QFile>
#include <QStandardPaths>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>

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
    if (!m_committed) {
        if (!m_database.rollback()) {
            qCInfo(appLog) << m_database.lastError();
        }
    }
}

void Transaction::commit()
{
    qCDebug(appLog) << "Committing transaction";
    m_committed = m_database.commit();
    if (!m_committed) {
        qCWarning(appLog) << "Failed to commit transaction:" << m_database.lastError();
    }
}


Database *Database::s_instance = nullptr;

Database *Database::instance()
{
    if (s_instance == nullptr) {
        s_instance = new Database(qApp);
    }
    return s_instance;
}

Database::~Database()
{
    m_database.close();
    s_instance = nullptr;
}

bool Database::prepareOperation()
{
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
                    ",currentPage INTEGER"
                    ",sidebarWidth INTEGER DEFAULT 200"
                    ",scrollPosition REAL DEFAULT 0.0"
                    ",expandedSections TEXT DEFAULT '[]'"
                    ",fileSize INTEGER DEFAULT 0"
                    ",lastModified INTEGER DEFAULT 0"
                    ",contentHash TEXT DEFAULT ''"
                    ")")) {
        qCInfo(appLog) << query.lastError();
        return false;
    }

    if (!query.isActive()) {
        qCInfo(appLog) << query.lastError();
        return false;
    }

    transaction.commit();
    return true;
}

bool Database::migrateOperationTable()
{
    qCDebug(appLog) << "Checking if operation table needs migration";
    QSqlQuery query(m_database);

    // 获取现有表的列信息
    if (!query.exec("PRAGMA table_info(operation)")) {
        qCWarning(appLog) << "Failed to get table info:" << query.lastError();
        return false;
    }

    QSet<QString> existingColumns;
    while (query.next()) {
        existingColumns.insert(query.value("name").toString());
    }

    // 需要添加的新列
    struct NewColumn {
        QString name;
        QString definition;
    };

    QList<NewColumn> newColumns = {
        {"sidebarWidth",  "INTEGER DEFAULT 200"},
        {"scrollPosition", "REAL DEFAULT 0.0"},
        {"expandedSections", "TEXT DEFAULT '[]'"},
        {"fileSize",       "INTEGER DEFAULT 0"},
        {"lastModified",   "INTEGER DEFAULT 0"},
        {"contentHash",    "TEXT DEFAULT ''"}
    };

    Transaction transaction(m_database);

    for (const auto &col : newColumns) {
        if (!existingColumns.contains(col.name)) {
            qCDebug(appLog) << "Migrating: adding column" << col.name;
            QString sql = QString("ALTER TABLE operation ADD COLUMN %1 %2")
                              .arg(col.name, col.definition);
            if (!query.exec(sql)) {
                QString errorText = query.lastError().text();
                if (errorText.contains("duplicate column name", Qt::CaseInsensitive)) {
                    qCDebug(appLog) << "Column already exists, skipping:" << col.name;
                } else {
                    qCWarning(appLog) << "Failed to add column" << col.name << ":" << errorText;
                }
            }
        }
    }

    transaction.commit();
    qCDebug(appLog) << "Operation table migration completed";
    return true;
}

bool Database::readOperation(DocSheet *sheet)
{
    if (!sheet) {
        return false;
    }

    QSqlQuery query(m_database);
    query.prepare("SELECT * FROM operation WHERE filePath = :filePath");
    query.bindValue(":filePath", sheet->filePath());
    if (!query.exec()) {
        qCWarning(appLog) << "Failed to read operation:" << query.lastError();
    }
    if (query.next()) {
        sheet->m_operation.layoutMode = static_cast<Dr::LayoutMode>(query.value("layoutMode").toInt());
        sheet->m_operation.mouseShape = static_cast<Dr::MouseShape>(query.value("mouseShape").toInt());
        sheet->m_operation.scaleMode = static_cast<Dr::ScaleMode>(query.value("scaleMode").toInt());
        sheet->m_operation.rotation = static_cast<Dr::Rotation>(query.value("rotation").toInt());
        sheet->m_operation.scaleFactor = qBound(0.1, query.value("scaleFactor").toDouble(), 5.0);
        sheet->m_operation.sidebarVisible = query.value("sidebarVisible").toInt();
        sheet->m_operation.sidebarIndex = query.value("sidebarIndex").toInt();
        sheet->m_operation.currentPage = query.value("currentPage").toInt();
        // V1.2 新增字段
        sheet->m_operation.sidebarWidth = query.value("sidebarWidth").toInt();
        sheet->m_operation.scrollPosition = query.value("scrollPosition").toFloat();
        // 目录树展开状态
        QString expandedJson = query.value("expandedSections").toString();
        QJsonDocument doc = QJsonDocument::fromJson(expandedJson.toUtf8());
        if (doc.isArray()) {
            QJsonArray arr = doc.array();
            for (const QJsonValue &val : arr) {
                sheet->m_operation.expandedSections.append(val.toString());
            }
        }
        return true;
    }
    return false;
}

bool Database::saveOperation(DocSheet *sheet, const QString &cachedContentHash)
{
    if (!sheet) {
        return false;
    }

    // 获取文件信息用于内容特征匹配
    QFileInfo fileInfo(sheet->filePath());
    qint64 fileSize = fileInfo.size();
    qint64 lastModified = fileInfo.lastModified().toMSecsSinceEpoch();
    // 如果传入了缓存哈希则直接使用，否则重新计算（避免自动保存时反复读盘）
    QString contentHash = cachedContentHash.isEmpty()
        ? computeContentHash(sheet->filePath())
        : cachedContentHash;

    QSqlQuery query(m_database);
    query.prepare("REPLACE INTO "
                  "operation(filePath,layoutMode,mouseShape,scaleMode,rotation,scaleFactor,"
                  "sidebarVisible,sidebarIndex,currentPage,sidebarWidth,scrollPosition,expandedSections,"
                  "fileSize,lastModified,contentHash)"
                  " VALUES(:filePath,:layoutMode,:mouseShape,:scaleMode,:rotation,:scaleFactor,"
                  ":sidebarVisible,:sidebarIndex,:currentPage,:sidebarWidth,:scrollPosition,:expandedSections,"
                  ":fileSize,:lastModified,:contentHash)");
    query.bindValue(":filePath", sheet->filePath());
    query.bindValue(":layoutMode", sheet->m_operation.layoutMode);
    query.bindValue(":mouseShape", sheet->m_operation.mouseShape);
    query.bindValue(":scaleMode", sheet->m_operation.scaleMode);
    query.bindValue(":rotation", sheet->m_operation.rotation);
    query.bindValue(":scaleFactor", sheet->m_operation.scaleFactor);
    query.bindValue(":sidebarVisible", sheet->m_operation.sidebarVisible);
    query.bindValue(":sidebarIndex", sheet->m_operation.sidebarIndex);
    query.bindValue(":currentPage", sheet->m_operation.currentPage);
    // V1.2 新增字段
    query.bindValue(":sidebarWidth", sheet->m_operation.sidebarWidth);
    query.bindValue(":scrollPosition", sheet->m_operation.scrollPosition);
    // 目录树展开状态序列化为 JSON 数组
    QJsonArray expandedArr;
    for (const QString &section : sheet->m_operation.expandedSections) {
        expandedArr.append(section);
    }
    query.bindValue(":expandedSections", QJsonDocument(expandedArr).toJson(QJsonDocument::Compact));
    query.bindValue(":fileSize", fileSize);
    query.bindValue(":lastModified", lastModified);
    query.bindValue(":contentHash", contentHash);

    if (!query.exec()) {
        qCInfo(appLog) << query.lastError().text();
        return false;
    }
    return true;
}

bool Database::matchOperationByContent(const QFileInfo &fileInfo, DocSheet *sheet)
{
    if (!sheet || !fileInfo.exists()) {
        return false;
    }

    qint64 fileSize = fileInfo.size();
    qint64 lastModified = fileInfo.lastModified().toMSecsSinceEpoch();
    QString contentHash = computeContentHash(fileInfo.absoluteFilePath());

    qCDebug(appLog) << "Matching by content: size=" << fileSize
                    << "mtime=" << lastModified << "hash=" << contentHash.left(16) << "...";

    Transaction matchTransaction(m_database);

    QSqlQuery query(m_database);
    // 匹配条件加入 lastModified，消除前 64KB 内容相同但实际不同的文件间的哈希碰撞
    query.prepare("SELECT * FROM operation WHERE fileSize = :fileSize "
                  "AND contentHash = :contentHash AND lastModified = :lastModified");
    query.bindValue(":fileSize", fileSize);
    query.bindValue(":contentHash", contentHash);
    query.bindValue(":lastModified", lastModified);

    if (!query.exec()) {
        qCWarning(appLog) << "Content match query failed:" << query.lastError();
        return false;
    }

    if (query.next()) {
        if (query.next()) {
            qCWarning(appLog) << "Multiple content matches found for size=" << fileSize
                              << ", refusing to update to avoid state overwrite";
            return false;
        }
        query.first();

        // 匹配成功，读取状态并更新文件路径
        QString oldFilePath = query.value("filePath").toString();
        qCInfo(appLog) << "Content match found: old path=" << oldFilePath << "-> new path=" << fileInfo.absoluteFilePath();

        sheet->m_operation.layoutMode = static_cast<Dr::LayoutMode>(query.value("layoutMode").toInt());
        sheet->m_operation.mouseShape = static_cast<Dr::MouseShape>(query.value("mouseShape").toInt());
        sheet->m_operation.scaleMode = static_cast<Dr::ScaleMode>(query.value("scaleMode").toInt());
        sheet->m_operation.rotation = static_cast<Dr::Rotation>(query.value("rotation").toInt());
        sheet->m_operation.scaleFactor = qBound(0.1, query.value("scaleFactor").toDouble(), 5.0);
        sheet->m_operation.sidebarVisible = query.value("sidebarVisible").toInt();
        sheet->m_operation.sidebarIndex = query.value("sidebarIndex").toInt();
        sheet->m_operation.currentPage = query.value("currentPage").toInt();
        sheet->m_operation.sidebarWidth = query.value("sidebarWidth").toInt();
        sheet->m_operation.scrollPosition = query.value("scrollPosition").toFloat();
        // 目录树展开状态
        QString expandedJson = query.value("expandedSections").toString();
        QJsonDocument expDoc = QJsonDocument::fromJson(expandedJson.toUtf8());
        if (expDoc.isArray()) {
            QJsonArray arr = expDoc.array();
            for (const QJsonValue &val : arr) {
                sheet->m_operation.expandedSections.append(val.toString());
            }
        }

        // 更新文件路径为新路径（在同一事务中保证原子性）
        QSqlQuery updateQuery(m_database);
        updateQuery.prepare("UPDATE operation SET filePath = :newPath WHERE filePath = :oldPath");
        updateQuery.bindValue(":newPath", fileInfo.absoluteFilePath());
        updateQuery.bindValue(":oldPath", oldFilePath);
        if (!updateQuery.exec()) {
            qCWarning(appLog) << "Failed to update file path after content match:" << updateQuery.lastError();
        }

        matchTransaction.commit();
        return true;
    }

    qCDebug(appLog) << "No content match found";
    return false;
}

int Database::cleanupOrphanStates()
{
    qCDebug(appLog) << "Starting orphan state cleanup";
    QSqlQuery query(m_database);
    if (!query.exec("SELECT filePath FROM operation")) {
        qCWarning(appLog) << "Failed to query operations for cleanup:" << query.lastError();
        return 0;
    }

    QStringList orphanPaths;
    while (query.next()) {
        QString filePath = query.value("filePath").toString();
        // 网络路径暂不清理
        if (Dr::isNetworkPath(filePath)) {
            continue;
        }
        if (!QFile::exists(filePath)) {
            orphanPaths.append(filePath);
        }
    }

    if (orphanPaths.isEmpty()) {
        qCDebug(appLog) << "No orphan states found";
        return 0;
    }

    Transaction transaction(m_database);
    QSqlQuery deleteQuery(m_database);
    int cleanedCount = 0;
    for (const QString &path : orphanPaths) {
        deleteQuery.prepare("DELETE FROM operation WHERE filePath = :filePath");
        deleteQuery.bindValue(":filePath", path);
        if (deleteQuery.exec()) {
            cleanedCount++;
            qCDebug(appLog) << "Cleaned orphan state:" << path;
        }
        // 同步清理书签（在同一事务中保证数据一致性）
        deleteQuery.prepare("DELETE FROM bookmark WHERE filePath = :filePath");
        deleteQuery.bindValue(":filePath", path);
        deleteQuery.exec();
    }
    transaction.commit();

    qCInfo(appLog) << "Cleaned" << cleanedCount << "orphan state records";
    return cleanedCount;
}

QString Database::computeContentHash(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(appLog) << "Cannot open file for hash:" << filePath;
        return QString();
    }

    // 读取前 64KB
    const qint64 maxBytes = 64 * 1024;
    QByteArray data = file.read(maxBytes);
    file.close();

    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

// ===== 标签页组持久化 =====

bool Database::prepareTabGroup()
{
    Transaction transaction(m_database);

    qCDebug(appLog) << "Preparing tabgroup table";
    QSqlQuery query(m_database);
    if (!query.exec("CREATE TABLE tabgroup "
                    "(groupId INTEGER PRIMARY KEY AUTOINCREMENT"
                    ",windowIndex INTEGER"
                    ",filePath TEXT"
                    ",tabOrder INTEGER"
                    ",active INTEGER DEFAULT 0)")) {
        qCWarning(appLog) << "Failed to create tabgroup table:" << query.lastError();
        return false;
    }

    if (!query.isActive()) {
        qCInfo(appLog) << query.lastError();
        return false;
    }

    transaction.commit();
    return true;
}

bool Database::saveTabGroup(int windowIndex, const QStringList &filePaths, int activeIndex)
{
    qCDebug(appLog) << "Saving tab group for window" << windowIndex << "with" << filePaths.size() << "files";

    Transaction transaction(m_database);

    // 先清除该窗口的旧记录（在同一事务中保证原子性）
    clearTabGroup(windowIndex);

    QSqlQuery query(m_database);

    for (int i = 0; i < filePaths.size(); ++i) {
        query.prepare("INSERT INTO tabgroup(windowIndex, filePath, tabOrder, active) "
                      "VALUES(:windowIndex, :filePath, :tabOrder, :active)");
        query.bindValue(":windowIndex", windowIndex);
        query.bindValue(":filePath", filePaths[i]);
        query.bindValue(":tabOrder", i);
        query.bindValue(":active", (i == activeIndex) ? 1 : 0);

        if (!query.exec()) {
            qCWarning(appLog) << "Failed to insert tab group record:" << query.lastError();
        }
    }

    transaction.commit();
    return true;
}

QStringList Database::readTabGroup(int windowIndex, int &activeIndex)
{
    qCDebug(appLog) << "Reading tab group for window" << windowIndex;

    QStringList filePaths;
    activeIndex = 0;

    QSqlQuery query(m_database);
    query.prepare("SELECT filePath, active FROM tabgroup "
                  "WHERE windowIndex = :windowIndex ORDER BY tabOrder ASC");
    query.bindValue(":windowIndex", windowIndex);

    if (!query.exec()) {
        qCWarning(appLog) << "Failed to read tab group:" << query.lastError();
        return filePaths;
    }

    int idx = 0;
    while (query.next()) {
        QString filePath = query.value("filePath").toString();
        filePaths.append(filePath);
        if (query.value("active").toInt() == 1) {
            activeIndex = idx;
        }
        idx++;
    }

    qCDebug(appLog) << "Read tab group:" << filePaths.size() << "files, active=" << activeIndex;
    return filePaths;
}

bool Database::clearTabGroup(int windowIndex)
{
    QSqlQuery query(m_database);
    query.prepare("DELETE FROM tabgroup WHERE windowIndex = :windowIndex");
    query.bindValue(":windowIndex", windowIndex);
    if (!query.exec()) {
        qCWarning(appLog) << "Failed to clear tab group:" << query.lastError();
        return false;
    }
    return true;
}

// ===== 原有书签功能不变 =====

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
    return true;
}

bool Database::readBookmarks(const QString &filePath, QSet<int> &bookmarks)
{
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

        return true;
    }

    return false;
}

bool Database::saveBookmarks(const QString &filePath, const QSet<int> bookmarks)
{
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

        return true;
    }

    return false;
}

// ===== 构造函数 =====

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
        } else {
            // 旧表存在，执行迁移
            migrateOperationTable();
        }

        if (!tables.contains("bookmark")) {
            prepareBookmark();
        }

        if (!tables.contains("tabgroup")) {
            prepareTabGroup();
        }
    } else {
        qCInfo(appLog) << m_database.lastError();
    }
    qCDebug(appLog) << "Database::Database() end";
}

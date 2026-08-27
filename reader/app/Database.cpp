// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Database.h"
#include "DocSheet.h"
#include "SheetRenderer.h"
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

#include <fcntl.h>
#include <unistd.h>
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
                    ",sidebarWidthChanged INTEGER DEFAULT 0"
                    ",scrollPosition REAL DEFAULT 0.0"
                    ",expandedSections TEXT DEFAULT '[]'"
                    ",fileSize INTEGER DEFAULT 0"
                    ",lastModified INTEGER DEFAULT 0"
                    ",contentHash TEXT DEFAULT ''"
                    ",docId TEXT DEFAULT ''"
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
        {"sidebarWidthChanged", "INTEGER DEFAULT 0"},
        {"scrollPosition", "REAL DEFAULT 0.0"},
        {"expandedSections", "TEXT DEFAULT '[]'"},
        {"fileSize",       "INTEGER DEFAULT 0"},
        {"lastModified",   "INTEGER DEFAULT 0"},
        {"contentHash",    "TEXT DEFAULT ''"},
        {"docId",          "TEXT DEFAULT ''"}
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
        sheet->m_operation.sidebarWidthChanged = query.value("sidebarWidthChanged").toInt() != 0;
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
    // 获取文档内置唯一标识符（如 PDF 的 /ID），用于文件移动/重命名后识别
    QString docId = sheet->m_renderer ? sheet->m_renderer->fileIdentifier() : QString();

    QSqlQuery query(m_database);
    query.prepare("REPLACE INTO "
                  "operation(filePath,layoutMode,mouseShape,scaleMode,rotation,scaleFactor,"
                  "sidebarVisible,sidebarIndex,currentPage,sidebarWidth,sidebarWidthChanged,scrollPosition,expandedSections,"
                  "fileSize,lastModified,contentHash,docId)"
                  " VALUES(:filePath,:layoutMode,:mouseShape,:scaleMode,:rotation,:scaleFactor,"
                  ":sidebarVisible,:sidebarIndex,:currentPage,:sidebarWidth,:sidebarWidthChanged,:scrollPosition,:expandedSections,"
                  ":fileSize,:lastModified,:contentHash,:docId)");
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
    query.bindValue(":sidebarWidthChanged", sheet->m_operation.sidebarWidthChanged ? 1 : 0);
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
    query.bindValue(":docId", docId);

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
    // 获取文档内置唯一标识符（PDF 的 /ID）
    QString docId = sheet->m_renderer ? sheet->m_renderer->fileIdentifier() : QString();

    qCDebug(appLog) << "Matching by content: size=" << fileSize
                    << "mtime=" << lastModified << "hash=" << contentHash.left(16) << "..."
                    << "docId=" << docId.left(16) << "...";

    Transaction matchTransaction(m_database);

    QVariantMap bestRow;

    // 优先通过 docId 匹配（PDF /ID，改名/移动不变，最可靠）
    if (!docId.isEmpty()) {
        QSqlQuery idQuery(m_database);
        idQuery.prepare("SELECT * FROM operation WHERE docId = :docId AND docId != ''");
        idQuery.bindValue(":docId", docId);
        if (idQuery.exec() && idQuery.next()) {
            for (int i = 0; i < idQuery.record().count(); ++i)
                bestRow.insert(idQuery.record().fieldName(i), idQuery.value(i));
            qint64 bestModTime = bestRow.value("lastModified").toLongLong();
            // docId 相同时选最近修改的记录
            while (idQuery.next()) {
                qint64 modTime = idQuery.value("lastModified").toLongLong();
                if (modTime > bestModTime) {
                    bestModTime = modTime;
                    bestRow.clear();
                    for (int i = 0; i < idQuery.record().count(); ++i)
                        bestRow.insert(idQuery.record().fieldName(i), idQuery.value(i));
                }
            }
            qCInfo(appLog) << "Matched by docId:" << docId.left(16) << "...";
        }
    }

    // docId 未命中时，回退到 fileSize + contentHash 匹配
    if (bestRow.isEmpty()) {
        QSqlQuery query(m_database);
        // 不使用 lastModified：保存注释会修改文件 mtime，导致移动文件后匹配失败
        query.prepare("SELECT * FROM operation WHERE fileSize = :fileSize "
                      "AND contentHash = :contentHash");
        query.bindValue(":fileSize", fileSize);
        query.bindValue(":contentHash", contentHash);

        if (!query.exec()) {
            qCWarning(appLog) << "Content match query failed:" << query.lastError();
            return false;
        }

        if (query.next()) {
            for (int i = 0; i < query.record().count(); ++i)
                bestRow.insert(query.record().fieldName(i), query.value(i));
            qint64 bestModTime = bestRow.value("lastModified").toLongLong();

            // 如果匹配到多条记录，选择最后修改时间最近的一条
            while (query.next()) {
                qint64 modTime = query.value("lastModified").toLongLong();
                if (modTime > bestModTime) {
                    bestModTime = modTime;
                    bestRow.clear();
                    for (int i = 0; i < query.record().count(); ++i)
                        bestRow.insert(query.record().fieldName(i), query.value(i));
                }
            }
        }
    }

    if (bestRow.isEmpty()) {
        qCDebug(appLog) << "No content match found";
        return false;
    }

    // 匹配成功，读取状态并更新文件路径
    QString oldFilePath = bestRow.value("filePath").toString();
    qCInfo(appLog) << "Content match found: old path=" << oldFilePath << "-> new path=" << fileInfo.absoluteFilePath();

    sheet->m_operation.layoutMode = static_cast<Dr::LayoutMode>(bestRow.value("layoutMode").toInt());
    sheet->m_operation.mouseShape = static_cast<Dr::MouseShape>(bestRow.value("mouseShape").toInt());
    sheet->m_operation.scaleMode = static_cast<Dr::ScaleMode>(bestRow.value("scaleMode").toInt());
    sheet->m_operation.rotation = static_cast<Dr::Rotation>(bestRow.value("rotation").toInt());
    sheet->m_operation.scaleFactor = qBound(0.1, bestRow.value("scaleFactor").toDouble(), 5.0);
    sheet->m_operation.sidebarVisible = bestRow.value("sidebarVisible").toInt();
    sheet->m_operation.sidebarIndex = bestRow.value("sidebarIndex").toInt();
    sheet->m_operation.currentPage = bestRow.value("currentPage").toInt();
    sheet->m_operation.sidebarWidth = bestRow.value("sidebarWidth").toInt();
    sheet->m_operation.sidebarWidthChanged = bestRow.value("sidebarWidthChanged").toInt() != 0;
    sheet->m_operation.scrollPosition = bestRow.value("scrollPosition").toFloat();
    // 目录树展开状态
    QString expandedJson = bestRow.value("expandedSections").toString();
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

    // 同时更新 bookmark 表的文件路径，确保书签数据也能跟随文件迁移
    QSqlQuery bookmarkUpdate(m_database);
    bookmarkUpdate.prepare("UPDATE bookmark SET filePath = :newPath WHERE filePath = :oldPath");
    bookmarkUpdate.bindValue(":newPath", fileInfo.absoluteFilePath());
    bookmarkUpdate.bindValue(":oldPath", oldFilePath);
    if (!bookmarkUpdate.exec()) {
        qCWarning(appLog) << "Failed to update bookmark path after content match:" << bookmarkUpdate.lastError();
    }

    matchTransaction.commit();
    return true;
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
            // 可移动设备路径（U盘等）暂不清理：设备未挂载时 QFile::exists() 返回 false，
            // 但文件仍在设备上，清理会导致重新挂载后数据丢失。
            // 通过检查路径是否在 /media、/mnt、/run/media 下判断是否为可移动设备
            if (filePath.startsWith("/media/") || filePath.startsWith("/mnt/")
                || filePath.startsWith("/run/media/")) {
                qCDebug(appLog) << "Skipping removable media path:" << filePath;
                continue;
            }
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

void Database::flushToDisk()
{
    if (!m_database.isOpen())
        return;

    // 1. checkpoint 将 WAL 内容合并到主数据库文件
    //    TRUNCATE 模式会在合并后截断 WAL 文件，并 fsync 主 DB 文件
    QSqlQuery query(m_database);
    if (!query.exec("PRAGMA wal_checkpoint(TRUNCATE)")) {
        qCWarning(appLog) << "Failed to checkpoint WAL:" << query.lastError();
    }

    // 2. 对数据库文件本身做 fsync，确保内核页缓存数据写入磁盘
    //    这是额外的安全措施，防止 checkpoint 的 fsync 未完全生效
    QString dbPath = m_database.databaseName();
    if (!dbPath.isEmpty()) {
        int fd = ::open(dbPath.toUtf8().constData(), O_RDONLY);
        if (fd >= 0) {
            ::fsync(fd);
            ::close(fd);
        }
    }
}

QString Database::computeContentHash(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qCWarning(appLog) << "Cannot open file for hash:" << filePath;
        return QString();
    }

    // 采样头部 64KB + 尾部 64KB，拼接后计算 SHA256
    // 纯头部 hash 在 WPS 等工具生成的 PDF 中容易碰撞（共享相同模板头）
    // 尾部包含 xref 表、trailer 等唯一信息，能显著提高区分度
    const qint64 chunkSize = 64 * 1024;
    qint64 fileSize = file.size();
    QByteArray data;

    // 头部
    data = file.read(chunkSize);

    // 尾部（文件 > 64KB 时追加尾部采样）
    if (fileSize > chunkSize) {
        file.seek(fileSize - chunkSize);
        data.append(file.read(chunkSize));
    }

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
            // 使用 WAL 模式：支持并发读取，崩溃后可通过 WAL 文件自动恢复
            if (!query.exec("PRAGMA journal_mode = WAL")) {
                qCWarning(appLog) << "Failed to set journal mode:" << query.lastError();
            }
            // FULL 模式：WAL 模式下每次 COMMIT 都 fsync WAL 文件，
            // 确保进程被杀后最新写入的事务不丢失（代价是每次写入慢 ~1ms）
            if (!query.exec("PRAGMA synchronous = FULL")) {
                qCWarning(appLog) << "Failed to set synchronous mode:" << query.lastError();
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

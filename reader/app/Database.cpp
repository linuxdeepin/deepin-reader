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
#include <QMultiMap>

// 网络文档阅读状态的保留时长：超过该天数未打开则视为过期，启动时清理
static constexpr int kNetworkStateTimeoutDays = 7;

/**
 * @brief isRemovableOrNetworkPath
 * 判断路径是否位于可移动设备或网络位置。此类路径未挂载/断开时
 * QFile::exists() 恒为 false，但文件实际仍在设备/服务器上，
 * 存在性判断不可靠，涉及记录合并/迁移时须保守处理
 */
static bool isRemovableOrNetworkPath(const QString &filePath)
{
    return Dr::isNetworkPath(filePath)
        || filePath.startsWith("/media/")
        || filePath.startsWith("/mnt/")
        || filePath.startsWith("/run/media/");
}

/**
 * @brief isPathProvablyGone
 * 判断路径上的文件是否确定已不存在（文件被移动/重命名的特征）。
 * 只有可靠的本地路径经 QFile::exists() 确认不存在才返回 true；
 * 可移动设备/网络路径因存在性不可靠，一律视为文件仍在
 */
static bool isPathProvablyGone(const QString &filePath)
{
    return !isRemovableOrNetworkPath(filePath) && !QFile::exists(filePath);
}

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
                    ",lastOpened INTEGER DEFAULT 0"
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

bool Database::migrateBookmarkTable()
{
    qCDebug(appLog) << "Checking if bookmark table needs migration";
    QSqlQuery query(m_database);

    if (!query.exec("PRAGMA table_info(bookmark)")) {
        qCWarning(appLog) << "Failed to get bookmark table info:" << query.lastError();
        return false;
    }

    QSet<QString> existingColumns;
    while (query.next()) {
        existingColumns.insert(query.value("name").toString());
    }

    if (existingColumns.contains("contentHash")) {
        return true;
    }

    Transaction transaction(m_database);
    if (!query.exec("ALTER TABLE bookmark ADD COLUMN contentHash TEXT DEFAULT ''")) {
        QString errorText = query.lastError().text();
        if (errorText.contains("duplicate column name", Qt::CaseInsensitive)) {
            qCDebug(appLog) << "Column already exists, skipping: contentHash";
        } else {
            qCWarning(appLog) << "Failed to add contentHash column to bookmark:" << errorText;
        }
    } else {
        qCInfo(appLog) << "Migrating bookmark table: added contentHash column";
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
        {"docId",          "TEXT DEFAULT ''"},
        {"lastOpened",     "INTEGER DEFAULT 0"}
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
        // 内容指纹校验：同名路径下换了文件（删除重命名、覆盖等）不应继承旧文档的阅读状态。
        // 校验不过则删除过期记录，让后续的 matchOperationByContent 有机会恢复新文档自己的状态
        const QString currentHash = computeContentHash(sheet->filePath());
        const QString recordHash = query.value("contentHash").toString();
        if (!currentHash.isEmpty() && !recordHash.isEmpty() && recordHash != currentHash) {
            qCInfo(appLog) << "Stale operation dropped (content mismatch):" << sheet->filePath();
            QSqlQuery delQuery(m_database);
            delQuery.prepare("DELETE FROM operation WHERE filePath = :filePath");
            delQuery.bindValue(":filePath", sheet->filePath());
            delQuery.exec();
            return false;
        }

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
        // 旧版本记录（无指纹）读取时回填，后续打开即可校验
        if (!currentHash.isEmpty() && recordHash.isEmpty()) {
            QSqlQuery backfill(m_database);
            backfill.prepare("UPDATE operation SET contentHash = :contentHash WHERE filePath = :filePath");
            backfill.bindValue(":contentHash", currentHash);
            backfill.bindValue(":filePath", sheet->filePath());
            if (!backfill.exec()) {
                qCWarning(appLog) << "Failed to backfill operation contentHash:" << backfill.lastError();
            }
        }

        // 同内容其他路径记录合并（仅限源文件已不存在的移动/重命名残留，
        // 见 mergeDuplicateRecords；源文件仍存在的记录保持独立，不参与合并）
        mergeDuplicateRecords(sheet);

        // 网络文档：命中即刷新打开时间，作为 7 天超时清理的时间基准
        if (Dr::isNetworkPath(sheet->filePath())) {
            QSqlQuery touchQuery(m_database);
            touchQuery.prepare("UPDATE operation SET lastOpened = :lastOpened WHERE filePath = :filePath");
            touchQuery.bindValue(":lastOpened", QDateTime::currentMSecsSinceEpoch());
            touchQuery.bindValue(":filePath", sheet->filePath());
            if (!touchQuery.exec()) {
                qCWarning(appLog) << "Failed to update lastOpened:" << touchQuery.lastError();
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
                  "fileSize,lastModified,contentHash,docId,lastOpened)"
                  " VALUES(:filePath,:layoutMode,:mouseShape,:scaleMode,:rotation,:scaleFactor,"
                  ":sidebarVisible,:sidebarIndex,:currentPage,:sidebarWidth,:sidebarWidthChanged,:scrollPosition,:expandedSections,"
                  ":fileSize,:lastModified,:contentHash,:docId,:lastOpened)");
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
    query.bindValue(":lastOpened", QDateTime::currentMSecsSinceEpoch());

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

    // 迁移池：源文件已确认消失（可靠的本地路径经存在性确认）→ 移动/重命名，记录整体迁移
    QVariantMap bestRow;
    // 借用池：源文件仍存在 → 复制到U盘等副本场景，仅首开借用状态，不动源记录
    QVariantMap borrowRow;
    auto rowToMap = [](const QSqlQuery &query) {
        QVariantMap row;
        for (int i = 0; i < query.record().count(); ++i)
            row.insert(query.record().fieldName(i), query.value(i));
        return row;
    };

    // 优先通过 docId 匹配（PDF /ID，改名/移动不变，最可靠）
    if (!docId.isEmpty()) {
        QSqlQuery idQuery(m_database);
        // 排除目标路径自身的记录：同一份文件可能在目标路径已有旧记录，
        // 选中它会导致 oldPath == newPath，做一次无效迁移
        idQuery.prepare("SELECT * FROM operation WHERE docId = :docId AND docId != '' AND filePath != :filePath");
        idQuery.bindValue(":docId", docId);
        idQuery.bindValue(":filePath", fileInfo.absoluteFilePath());
        if (idQuery.exec()) {
            // docId 相同时选最近修改的记录。
            // 源文件已消失的进入迁移池；仍存在的进入借用池：
            // 同一份内容在多个路径共存时不得夺走源路径的记录，
            // 但新路径首次打开（自身无记录）可借用一份源状态
            while (idQuery.next()) {
                const QVariantMap row = rowToMap(idQuery);
                const qint64 modTime = row.value("lastModified").toLongLong();
                if (!isPathProvablyGone(row.value("filePath").toString())) {
                    if (borrowRow.isEmpty() || modTime > borrowRow.value("lastModified").toLongLong())
                        borrowRow = row;
                    continue;
                }
                if (bestRow.isEmpty() || modTime > bestRow.value("lastModified").toLongLong())
                    bestRow = row;
            }
            if (!bestRow.isEmpty()) {
                qCInfo(appLog) << "Matched by docId:" << docId.left(16) << "...";
            }
        }
    }

    // docId 未命中时，回退到 fileSize + contentHash 匹配
    if (bestRow.isEmpty()) {
        QSqlQuery query(m_database);
        // 不使用 lastModified：保存注释会修改文件 mtime，导致移动文件后匹配失败
        // 排除目标路径自身记录，理由同 docId 分支
        query.prepare("SELECT * FROM operation WHERE fileSize = :fileSize "
                      "AND contentHash = :contentHash AND filePath != :filePath");
        query.bindValue(":fileSize", fileSize);
        query.bindValue(":contentHash", contentHash);
        query.bindValue(":filePath", fileInfo.absoluteFilePath());

        if (!query.exec()) {
            qCWarning(appLog) << "Content match query failed:" << query.lastError();
            return false;
        }

        // 匹配到多条记录时选最后修改时间最近的一条；
        // 源文件已消失的进入迁移池，仍存在的进入借用池（理由同 docId 分支）
        while (query.next()) {
            const QVariantMap row = rowToMap(query);
            const qint64 modTime = row.value("lastModified").toLongLong();
            if (!isPathProvablyGone(row.value("filePath").toString())) {
                if (borrowRow.isEmpty() || modTime > borrowRow.value("lastModified").toLongLong())
                    borrowRow = row;
                continue;
            }
            if (bestRow.isEmpty() || modTime > bestRow.value("lastModified").toLongLong())
                bestRow = row;
        }
    }

    if (bestRow.isEmpty() && borrowRow.isEmpty()) {
        qCDebug(appLog) << "No content match found";
        return false;
    }

    // 迁移优先于借用：存在源文件已消失的候选时整体迁移（移动/重命名场景）
    const QVariantMap row = bestRow.isEmpty() ? borrowRow : bestRow;
    const bool borrowOnly = bestRow.isEmpty();

    QString oldFilePath = row.value("filePath").toString();
    qCInfo(appLog) << (borrowOnly ? "First open of a copy, borrowing state from source: "
                                  : "Content match found: old path=")
                   << oldFilePath << "-> new path=" << fileInfo.absoluteFilePath();

    sheet->m_operation.layoutMode = static_cast<Dr::LayoutMode>(row.value("layoutMode").toInt());
    sheet->m_operation.mouseShape = static_cast<Dr::MouseShape>(row.value("mouseShape").toInt());
    sheet->m_operation.scaleMode = static_cast<Dr::ScaleMode>(row.value("scaleMode").toInt());
    sheet->m_operation.rotation = static_cast<Dr::Rotation>(row.value("rotation").toInt());
    sheet->m_operation.scaleFactor = qBound(0.1, row.value("scaleFactor").toDouble(), 5.0);
    sheet->m_operation.sidebarVisible = row.value("sidebarVisible").toInt();
    sheet->m_operation.sidebarIndex = row.value("sidebarIndex").toInt();
    sheet->m_operation.currentPage = row.value("currentPage").toInt();
    sheet->m_operation.sidebarWidth = row.value("sidebarWidth").toInt();
    sheet->m_operation.sidebarWidthChanged = row.value("sidebarWidthChanged").toInt() != 0;
    sheet->m_operation.scrollPosition = row.value("scrollPosition").toFloat();
    // 目录树展开状态
    QString expandedJson = row.value("expandedSections").toString();
    QJsonDocument expDoc = QJsonDocument::fromJson(expandedJson.toUtf8());
    if (expDoc.isArray()) {
        QJsonArray arr = expDoc.array();
        for (const QJsonValue &val : arr) {
            sheet->m_operation.expandedSections.append(val.toString());
        }
    }

    // 首开借用（源文件仍存在）：不迁移、不删除源路径的任何记录。
    // 仅把匹配指纹的书签复制一份到新路径（源路径书签行原样保留），
    // 保证 setAlive 后续 readBookmarks 能直接读到；关闭时
    // saveOperation/saveBookmarks 照常落盘新路径自己的记录，
    // 此后两份文档各自独立，"借用"仅发生在首开这一次
    if (borrowOnly) {
        QSqlQuery clearBm(m_database);
        clearBm.prepare("DELETE FROM bookmark WHERE filePath = :newPath "
                        "AND (contentHash = :contentHash OR contentHash = '')");
        clearBm.bindValue(":newPath", fileInfo.absoluteFilePath());
        clearBm.bindValue(":contentHash", row.value("contentHash").toString());
        if (!clearBm.exec()) {
            qCWarning(appLog) << "Failed to clear stale target bookmarks before borrowing:"
                              << clearBm.lastError();
        }
        QSqlQuery copyBm(m_database);
        // 同名占位符在 Qt SQLite 驱动下仅绑定首个出现位置，需拆名绑定
        copyBm.prepare("INSERT INTO bookmark(filePath, bookmarkIndex, contentHash) "
                       "SELECT :newPath, bookmarkIndex, contentHash FROM bookmark "
                       "WHERE filePath = :oldPath "
                       "AND (contentHash = :contentHash0 OR contentHash = :contentHash1)");
        copyBm.bindValue(":newPath", fileInfo.absoluteFilePath());
        copyBm.bindValue(":oldPath", oldFilePath);
        copyBm.bindValue(":contentHash0", row.value("contentHash").toString());
        copyBm.bindValue(":contentHash1", row.value("contentHash").toString());
        if (!copyBm.exec()) {
            qCWarning(appLog) << "Failed to copy bookmarks while borrowing state:"
                              << copyBm.lastError();
        }
        matchTransaction.commit();
        return true;
    }

    // 目标路径若残留旧 operation 记录（同名文档历史记录），先删除避让主键，
    // 否则下方 UPDATE 会因 operation.filePath 主键冲突而失败
    // （readOperation miss 已删除过期记录，此处为防御性清理，不影响未过期数据）
    QSqlQuery clearQuery(m_database);
    clearQuery.prepare("DELETE FROM operation WHERE filePath = :newPath AND filePath != :oldPath");
    clearQuery.bindValue(":newPath", fileInfo.absoluteFilePath());
    clearQuery.bindValue(":oldPath", oldFilePath);
    if (!clearQuery.exec()) {
        qCWarning(appLog) << "Failed to clear stale record before path update:" << clearQuery.lastError();
    }

    // 更新文件路径为新路径（在同一事务中保证原子性）
    QSqlQuery updateQuery(m_database);
    updateQuery.prepare("UPDATE operation SET filePath = :newPath, lastOpened = :lastOpened WHERE filePath = :oldPath");
    updateQuery.bindValue(":newPath", fileInfo.absoluteFilePath());
    updateQuery.bindValue(":oldPath", oldFilePath);
    updateQuery.bindValue(":lastOpened", QDateTime::currentMSecsSinceEpoch());
    if (!updateQuery.exec()) {
        qCWarning(appLog) << "Failed to update file path after content match:" << updateQuery.lastError();
    }

    // 书签以「路径 + 内容指纹」为关联键，跨路径不会自动共享：
    // 源路径文件已不存在（判定为移动/重命名），旧路径书签一并迁移到新路径
    QSqlQuery bookmarkQuery(m_database);
    bookmarkQuery.prepare("UPDATE bookmark SET filePath = :newPath "
                          "WHERE filePath = :oldPath "
                          "AND (contentHash = :contentHash OR contentHash = '')");
    bookmarkQuery.bindValue(":newPath", fileInfo.absoluteFilePath());
    bookmarkQuery.bindValue(":oldPath", oldFilePath);
    bookmarkQuery.bindValue(":contentHash", bestRow.value("contentHash").toString());
    if (!bookmarkQuery.exec()) {
        qCWarning(appLog) << "Failed to migrate bookmarks after content match:" << bookmarkQuery.lastError();
    }

    matchTransaction.commit();
    return true;
}

bool Database::mergeDuplicateRecords(DocSheet *sheet)
{
    if (!sheet) {
        return false;
    }

    const QString newPath = sheet->filePath();

    // 读取当前记录的内容特征（指纹已在 readOperation 中校验通过）
    QSqlQuery curQuery(m_database);
    curQuery.prepare("SELECT fileSize, contentHash, docId, lastOpened FROM operation WHERE filePath = :filePath");
    curQuery.bindValue(":filePath", newPath);
    if (!curQuery.exec() || !curQuery.next()) {
        return false;
    }
    const qint64 fileSize = curQuery.value("fileSize").toLongLong();
    const QString contentHash = curQuery.value("contentHash").toString();
    const QString docId = curQuery.value("docId").toString();
    qint64 curLastOpened = curQuery.value("lastOpened").toLongLong();

    // 无指纹无法可靠判定同内容，不做合并
    if (contentHash.isEmpty()) {
        return false;
    }

    // 查找同内容的其他路径记录：docId 相同（非空）或 fileSize+指纹相同
    QSqlQuery dupQuery(m_database);
    dupQuery.prepare("SELECT * FROM operation WHERE filePath != :newPath AND "
                     "((docId != '' AND docId = :docId) OR "
                     "(fileSize = :fileSize AND contentHash = :contentHash))");
    dupQuery.bindValue(":newPath", newPath);
    dupQuery.bindValue(":docId", docId);
    dupQuery.bindValue(":fileSize", fileSize);
    dupQuery.bindValue(":contentHash", contentHash);
    if (!dupQuery.exec()) {
        qCWarning(appLog) << "Failed to query duplicate content records:" << dupQuery.lastError();
        return false;
    }

    QVariantList dupRows;
    while (dupQuery.next()) {
        QVariantMap row;
        for (int i = 0; i < dupQuery.record().count(); ++i) {
            row.insert(dupQuery.record().fieldName(i), dupQuery.value(i));
        }
        dupRows.append(row);
    }

    if (dupRows.isEmpty()) {
        return false;
    }

    // 当前路径书签与同内容记录独立（书签以 路径+指纹 为关联键），
    // 源文件已不存在的旧路径书签需并入当前路径，阅读状态按 lastOpened 择优

    qCInfo(appLog) << "Merging duplicate content records into:" << newPath << "count=" << dupRows.count();

    Transaction mergeTransaction(m_database);

    bool mergedAny = false;
    for (const QVariant &rowVar : dupRows) {
        const QVariantMap dup = rowVar.toMap();
        const QString oldPath = dup.value("filePath").toString();
        if (oldPath == newPath) {
            continue;
        }

        // 源文件仍存在（或路径存在性不可靠）说明不是移动/重命名场景，
        // 而是同一份内容在多个路径共存：各路径保持独立阅读状态，不合并、不删除
        if (!isPathProvablyGone(oldPath)) {
            qCInfo(appLog) << "Keep independent state, source file still present:" << oldPath;
            continue;
        }

        // 阅读状态择优：取 lastOpened 较新者（书签已随内容指纹共享，无需并入）
        const qint64 dupLastOpened = dup.value("lastOpened").toLongLong();
        const bool dupPreferred = dupLastOpened > curLastOpened;

        if (dupPreferred) {
            QSqlQuery updQuery(m_database);
            updQuery.prepare("UPDATE operation SET layoutMode = :layoutMode, mouseShape = :mouseShape, "
                             "scaleMode = :scaleMode, rotation = :rotation, scaleFactor = :scaleFactor, "
                             "sidebarVisible = :sidebarVisible, sidebarIndex = :sidebarIndex, "
                             "currentPage = :currentPage, sidebarWidth = :sidebarWidth, "
                             "sidebarWidthChanged = :sidebarWidthChanged, scrollPosition = :scrollPosition, "
                             "expandedSections = :expandedSections, lastModified = :lastModified, "
                             "lastOpened = :lastOpened WHERE filePath = :newPath");
            updQuery.bindValue(":layoutMode", dup.value("layoutMode"));
            updQuery.bindValue(":mouseShape", dup.value("mouseShape"));
            updQuery.bindValue(":scaleMode", dup.value("scaleMode"));
            updQuery.bindValue(":rotation", dup.value("rotation"));
            updQuery.bindValue(":scaleFactor", dup.value("scaleFactor"));
            updQuery.bindValue(":sidebarVisible", dup.value("sidebarVisible"));
            updQuery.bindValue(":sidebarIndex", dup.value("sidebarIndex"));
            updQuery.bindValue(":currentPage", dup.value("currentPage"));
            updQuery.bindValue(":sidebarWidth", dup.value("sidebarWidth"));
            updQuery.bindValue(":sidebarWidthChanged", dup.value("sidebarWidthChanged"));
            updQuery.bindValue(":scrollPosition", dup.value("scrollPosition"));
            updQuery.bindValue(":expandedSections", dup.value("expandedSections"));
            updQuery.bindValue(":lastModified", dup.value("lastModified"));
            updQuery.bindValue(":lastOpened", dup.value("lastOpened"));
            updQuery.bindValue(":newPath", newPath);
            if (updQuery.exec()) {
                // 同步到内存对象，本次会话直接使用合并后的状态
                sheet->m_operation.layoutMode = static_cast<Dr::LayoutMode>(dup.value("layoutMode").toInt());
                sheet->m_operation.mouseShape = static_cast<Dr::MouseShape>(dup.value("mouseShape").toInt());
                sheet->m_operation.scaleMode = static_cast<Dr::ScaleMode>(dup.value("scaleMode").toInt());
                sheet->m_operation.rotation = static_cast<Dr::Rotation>(dup.value("rotation").toInt());
                sheet->m_operation.scaleFactor = qBound(0.1, dup.value("scaleFactor").toDouble(), 5.0);
                sheet->m_operation.sidebarVisible = dup.value("sidebarVisible").toInt();
                sheet->m_operation.sidebarIndex = dup.value("sidebarIndex").toInt();
                sheet->m_operation.currentPage = dup.value("currentPage").toInt();
                sheet->m_operation.sidebarWidth = dup.value("sidebarWidth").toInt();
                sheet->m_operation.sidebarWidthChanged = dup.value("sidebarWidthChanged").toInt() != 0;
                sheet->m_operation.scrollPosition = dup.value("scrollPosition").toFloat();
                QString expandedJson = dup.value("expandedSections").toString();
                QJsonDocument expDoc = QJsonDocument::fromJson(expandedJson.toUtf8());
                if (expDoc.isArray()) {
                    sheet->m_operation.expandedSections.clear();
                    for (const QJsonValue &val : expDoc.array()) {
                        sheet->m_operation.expandedSections.append(val.toString());
                    }
                }
                curLastOpened = dupLastOpened;
            } else {
                qCWarning(appLog) << "Failed to apply duplicate record state:" << updQuery.lastError();
            }
        }

        // 源路径文件已不存在（移动/重命名残留）：其书签并入当前路径
        // （书签以 路径+指纹 为关联键，跨路径不会自动共享，须显式迁移；
        //  与记录指纹不符的旧书签留在原路径，由无主书签清理兜底）
        QSqlQuery bmQuery(m_database);
        bmQuery.prepare("UPDATE bookmark SET filePath = :newPath "
                        "WHERE filePath = :oldPath "
                        "AND (contentHash = :contentHash OR contentHash = '')");
        bmQuery.bindValue(":newPath", newPath);
        bmQuery.bindValue(":oldPath", oldPath);
        bmQuery.bindValue(":contentHash", dup.value("contentHash").toString());
        if (!bmQuery.exec()) {
            qCWarning(appLog) << "Failed to migrate bookmarks during merge:" << bmQuery.lastError();
        }

        // 删除旧路径 operation 记录（书签已并入当前路径）
        QSqlQuery delQuery(m_database);
        delQuery.prepare("DELETE FROM operation WHERE filePath = :oldPath");
        delQuery.bindValue(":oldPath", oldPath);
        if (delQuery.exec()) {
            mergedAny = true;
        } else {
            qCWarning(appLog) << "Failed to remove duplicate record:" << delQuery.lastError();
        }
    }

    if (!mergedAny) {
        // 源文件均仍存在：同内容多路径独立共存，保留各自记录
        qCInfo(appLog) << "No duplicate merged, all source files still present:" << newPath;
        return false;
    }

    mergeTransaction.commit();
    qCInfo(appLog) << "Duplicate content records merged: path=" << newPath;
    return true;
}

int Database::cleanupOrphanStates()
{
    qCDebug(appLog) << "Starting orphan state cleanup";
    QSqlQuery query(m_database);
    // 同时取出 lastOpened、contentHash，用于保留判定与超时判断
    if (!query.exec("SELECT filePath, lastOpened, contentHash FROM operation")) {
        qCWarning(appLog) << "Failed to query operations for cleanup:" << query.lastError();
        return 0;
    }

    // 网络文档状态的超时时间（毫秒）
    const qint64 networkTimeoutMs = qint64(kNetworkStateTimeoutDays) * 24 * 60 * 60 * 1000;
    const qint64 now = QDateTime::currentMSecsSinceEpoch();

    QStringList orphanPaths;
    while (query.next()) {
        QString filePath = query.value("filePath").toString();
        if (Dr::isNetworkPath(filePath)) {
            // 网络路径：挂载状态不可靠（QFile::exists() 无法判断文件是否仍存在），
            // 改用超时策略——超过 7 天未打开的网络文档状态记录视为过期，连同书签一起清理
            const qint64 lastOpened = query.value("lastOpened").toLongLong();
            if (lastOpened > 0 && now - lastOpened > networkTimeoutMs) {
                qCInfo(appLog) << "Network document state expired (lastOpened"
                                << QDateTime::fromMSecsSinceEpoch(lastOpened).toString() << "):" << filePath;
                orphanPaths.append(filePath);
            }
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
            // 文件可能只是被重命名/移动：带内容指纹的记录不立即删除，
            // 保留供打开新路径时通过 matchOperationByContent 按指纹迁移
            // （阅读进度与书签随迁移更新路径）；长期未打开则按 7 天超时清理。
            // 无指纹的旧格式记录无法参与内容匹配，维持原立即清理策略。
            const qint64 lastOpened = query.value("lastOpened").toLongLong();
            const QString contentHash = query.value("contentHash").toString();
            if (contentHash.isEmpty() || lastOpened <= 0
                || now - lastOpened > networkTimeoutMs) {
                qCInfo(appLog) << "Orphan document state cleanup (no fingerprint or expired):" << filePath;
                orphanPaths.append(filePath);
            } else {
                qCInfo(appLog) << "Keeping missing-file state for content re-match (lastOpened"
                                << QDateTime::fromMSecsSinceEpoch(lastOpened).toString() << "):" << filePath;
            }
            continue;
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

    // 书签以内容指纹为关联键后，其生命周期与内容对齐：
    // 同一指纹的 operation 记录已全部消失（该内容不再有任何阅读记录），
    // 且书签记录挂载的所有路径文件均已不存在，则视为无主书签，一并清理。
    // 任一挂载路径文件仍存在（如同名覆盖后原文件从回收站恢复）则保留，
    // 待用户打开后按指纹自动找回；可移动设备/网络路径不可靠（可能未挂载），跳过。
    // 注意需在上方 operation 孤立清理之后执行，判定结果才反映清理后的状态。
    cleanedCount += cleanupOrphanBookmarks();

    qCInfo(appLog) << "Cleaned" << cleanedCount << "orphan state records";
    return cleanedCount;
}

int Database::cleanupOrphanBookmarks()
{
    // 1) 找出已无任何 operation 记录对应的指纹（该内容不再有阅读记录）
    QSqlQuery hashQuery(m_database);
    if (!hashQuery.exec("SELECT DISTINCT contentHash, filePath FROM bookmark "
                        "WHERE contentHash != '' AND contentHash NOT IN "
                        "(SELECT DISTINCT contentHash FROM operation)")) {
        qCWarning(appLog) << "Failed to query orphan bookmark hashes:" << hashQuery.lastError();
        return 0;
    }

    // hash -> 挂载路径集合
    QMultiMap<QString, QString> orphanCandidates;
    while (hashQuery.next()) {
        orphanCandidates.insert(hashQuery.value(0).toString(), hashQuery.value(1).toString());
    }
    if (orphanCandidates.isEmpty()) {
        return 0;
    }

    // 2) 逐指纹判定：所有挂载路径的文件均不存在（且不含不可靠路径）才清理
    QStringList deadHashes;
    for (auto it = orphanCandidates.constBegin(); it != orphanCandidates.constEnd(); ++it) {
        bool allGone = true;
        for (auto pathIt = orphanCandidates.lowerBound(it.key());
             pathIt != orphanCandidates.upperBound(it.key()); ++pathIt) {
            const QString &path = pathIt.value();
            if (Dr::isNetworkPath(path)
                || path.startsWith("/media/") || path.startsWith("/mnt/")
                || path.startsWith("/run/media/")) {
                allGone = false;
                break;
            }
            if (QFile::exists(path)) {
                allGone = false;
                break;
            }
        }
        if (allGone) {
            deadHashes << it.key();
        }
    }
    if (deadHashes.isEmpty()) {
        return 0;
    }

    Transaction transaction(m_database);
    QSqlQuery deleteQuery(m_database);
    int removed = 0;
    for (const QString &hash : deadHashes) {
        deleteQuery.prepare("DELETE FROM bookmark WHERE contentHash = :contentHash");
        deleteQuery.bindValue(":contentHash", hash);
        if (deleteQuery.exec()) {
            removed += qMax(0, deleteQuery.numRowsAffected());
            qCInfo(appLog) << "Cleaned orphan bookmarks (content no longer tracked): hash="
                           << hash.left(8) << "rows=" << deleteQuery.numRowsAffected();
        }
    }
    transaction.commit();
    return removed;
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
        // 注意必须返回非 null 空串：QString() 是 null string，经 QVariant 绑定到
        // SQLite 会被当作 NULL，导致调用方的 hash 等值比较全部失效
        return QString(QLatin1String(""));
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
    if (!query.exec("CREATE TABLE bookmark(filePath TEXT,bookmarkIndex INTEGER,contentHash TEXT DEFAULT '')")) {
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

        // 书签以「路径 + 内容指纹」为关联键，而非仅内容指纹：
        // 1) 同一份内容在多个路径共存（本地+U盘拷贝等）时，各路径书签相互独立，
        //    在任一路径增删书签不会同步到其他路径的同内容文档上；
        // 2) 文件移动/重命名（源路径文件已不存在）时，书签随阅读状态一起
        //    迁移到新路径（matchOperationByContent / mergeDuplicateRecords），
        //    移动后打开新路径仍可找回书签；
        // 3) 文件内容分叉（如保存高亮注释后指纹变化）后，旧指纹的书签保留在库中，
        //    原始文件再打开仍可找回，新书签写入新指纹，两者互不干扰；
        // 4) 同名路径被另一个不同内容的文件覆盖后，旧书签读不出来但保留，
        //    原文件回来后书签自动恢复。
        // filePath 仅用于兜底：a) 匹配无指纹的旧版本数据（legacy，读取后回填指纹）；
        // b) 当前文件不可读（指纹无法计算）时退回按路径读取全部书签（与旧版本行为一致）。
        // 注意：Qt SQLite 驱动对同名占位符多次出现只绑定第一处，
        // 同一变量需拆成多个不同名的占位符分别绑定
        if (!query.prepare("SELECT contentHash, bookmarkIndex FROM bookmark "
                           "WHERE (filePath = :filePath AND contentHash = :currentHash) "
                           "OR (:currentHash0 = '' AND filePath = :filePath0) "
                           "OR (filePath = :filePath1 AND contentHash = '')")) {
            qCInfo(appLog) << query.lastError();
            return false;
        }
        const QString currentHash = computeContentHash(filePath);
        query.bindValue(":currentHash", currentHash);
        query.bindValue(":currentHash0", currentHash);
        query.bindValue(":filePath", filePath);
        query.bindValue(":filePath0", filePath);
        query.bindValue(":filePath1", filePath);

        if (!query.exec()) {
            qCInfo(appLog) << query.lastError().text();
            return false;
        }

        // 旧版本数据（无指纹）：批量回填当前文件指纹，后续打开即可按指纹读取
        // （backfill 使 legacy 书签并入当前内容指纹的书签世界，
        //  放在遍历结束后执行，避免在 SELECT 迭代过程中写库）
        bool hasLegacyRows = false;
        while (query.next()) {
            bookmarks.insert(query.value("bookmarkIndex").toInt());
            if (query.value("contentHash").toString().isEmpty()) {
                hasLegacyRows = true;
            }
        }

        if (hasLegacyRows && !currentHash.isEmpty()) {
            QSqlQuery backfill(m_database);
            backfill.prepare("UPDATE bookmark SET contentHash = :contentHash "
                             "WHERE filePath = :filePath AND contentHash = ''");
            backfill.bindValue(":contentHash", currentHash);
            backfill.bindValue(":filePath", filePath);
            if (!backfill.exec()) {
                qCWarning(appLog) << "Failed to backfill bookmark contentHash:" << backfill.lastError();
            }
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

        // 记录当前文件内容指纹：书签以「路径 + 内容指纹」为关联键（跟路径下的内容走）
        const QString contentHash = computeContentHash(filePath);

        if (!contentHash.isEmpty()) {
            // 按 路径+指纹 全量替换：仅删除当前路径当前指纹（及无指纹 legacy 行）的书签。
            // 关键一：其他路径同内容拷贝的书签不受影响（各路径独立，不互相同步）；
            // 关键二：同一路径其它指纹（历史内容版本）的书签保留 ——
            // 文件保存高亮注释等内容变化后，旧版本书签仍留存，原始文件/历史版本可找回。
            if (!query.prepare("DELETE FROM bookmark WHERE filePath = :filePath "
                               "AND (contentHash = :contentHash OR contentHash = '')")) {
                qCInfo(appLog) << query.lastError();
                return false;
            }
        } else {
            // 指纹不可用（文件不可读等）：退回路径维度删除（legacy 行为）
            if (!query.prepare("DELETE FROM bookmark WHERE filePath = :filePath")) {
                qCInfo(appLog) << query.lastError();
                return false;
            }
        }

        query.bindValue(":contentHash", contentHash);
        query.bindValue(":filePath", filePath);

        if (!query.exec()) {
            qCInfo(appLog) << query.lastError().text();
            return false;
        }

        foreach (int index, bookmarks) {
            if (!query.prepare(" insert into "
                               " bookmark(filePath,bookmarkIndex,contentHash)"
                               " VALUES(:filePath,:bookmarkIndex,:contentHash)")) {
                qCInfo(appLog) << query.lastError();
                return false;
            }

            query.bindValue(":filePath", filePath);
            query.bindValue(":bookmarkIndex", index);
            query.bindValue(":contentHash", contentHash);

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
        } else {
            // 旧书签表迁移：增加内容指纹列
            migrateBookmarkTable();
        }

        if (!tables.contains("tabgroup")) {
            prepareTabGroup();
        }
    } else {
        qCInfo(appLog) << m_database.lastError();
    }
    qCDebug(appLog) << "Database::Database() end";
}

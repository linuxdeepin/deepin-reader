// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSet>
#include <QStringList>
#include <QFileInfo>

class DocSheet;
class Sheet;
class QDateTime;
class DocumentView;

class Transaction
{
public:
    explicit Transaction(QSqlDatabase &database);
    ~Transaction();
    void commit();

private:
    bool m_committed;
    QSqlDatabase &m_database;

};

class Database : public QObject
{
    Q_OBJECT

public:
    static Database *instance();

    ~Database();

    /**
     * @brief readOperation
     * 读取操作
     * @param sheet 哪个文档
     * @return
     */
    bool readOperation(DocSheet *sheet);

    /**
     * @brief saveOperation
     * 保存操作
     * @param sheet 哪个文档
     * @return
     */
    bool saveOperation(DocSheet *sheet, const QString &cachedContentHash = QString());

    /**
     * @brief readBookmarks
     * 读取书签（以 路径+内容指纹 为关联键：同内容文件在不同路径各自独立一套书签，
     * 互不同步；文件移动/重命名后书签随阅读状态迁移到新路径；
     * 内容分叉（保存注释）后各版本书签独立保留；同名不同内容的文件不会读到无关书签）
     * @param filePath 文件名(与指纹共同作为关联键，另用于 legacy 无指纹书签兜底)
     * @param bookmarks 书签列表
     * @return
     */
    bool readBookmarks(const QString &filePath, QSet<int> &bookmarks);

    /**
     * @brief saveBookmarks
     * 保存书签（按 路径+指纹 全量替换该书签集合；
     * 其他路径同内容拷贝的书签不受影响；同路径其它内容指纹的
     * 历史版本书签保留，不随保存删除）
     * @param filePath 文件名(与指纹共同作为关联键)
     * @param bookmarks 书签列表
     * @return
     */
    bool saveBookmarks(const QString &filePath, const QSet<int> bookmarks);

    // ===== 标签页组持久化 =====

    /**
     * @brief saveTabGroup
     * 保存标签页组（窗口打开的文档列表及顺序）
     * @param windowIndex 窗口序号
     * @param filePaths 文档路径列表（按标签页顺序）
     * @param activeIndex 活动标签页索引
     * @return
     */
    bool saveTabGroup(int windowIndex, const QStringList &filePaths, int activeIndex);

    /**
     * @brief readTabGroup
     * 读取标签页组
     * @param windowIndex 窗口序号
     * @param activeIndex 传出：活动标签页索引
     * @return 文档路径列表
     */
    QStringList readTabGroup(int windowIndex, int &activeIndex);

    /**
     * @brief clearTabGroup
     * 清除指定窗口的标签页组记录
     * @param windowIndex 窗口序号
     * @return
     */
    bool clearTabGroup(int windowIndex);

    // ===== 内容特征匹配 =====

    /**
     * @brief matchOperationByContent
     * 通过文件内容特征（docId 优先，其次 fileSize+内容哈希）匹配已保存的操作记录，
     * 用于目标路径自身无记录时恢复阅读状态。两种命中方式：
     * 1. 迁移：源文件已确认不存在（可靠的本地路径经存在性确认），
     *    视为移动/重命名，记录与书签整体迁移到新路径；
     * 2. 首开借用：源文件仍存在（复制到U盘等副本场景），仅把源状态
     *    与匹配指纹的书签复制一份给新路径，源路径记录一行不动 ——
     *    新路径关闭落盘自己的记录后与源路径各自独立，避免互相覆盖
     * @param fileInfo 文件信息
     * @param sheet 目标sheet，匹配成功后写入其operation
     * @return 是否匹配成功
     */
    bool matchOperationByContent(const QFileInfo &fileInfo, DocSheet *sheet);

    // ===== 孤立状态清理 =====

    /**
     * @brief cleanupOrphanStates
     * 清理已不存在的本地文档对应的状态记录；
     * 带内容指纹的记录不立即删除（文件可能只是被重命名/移动，
     * 需保留供打开新路径时按指纹迁移），改用与网络文档一致的
     * 7 天超时策略；无指纹的旧格式记录维持原删除策略；
     * 网络文档按超时清理：超过 7 天未打开的记录（含书签）被清除。
     * 书签以内容指纹为关联键后，另追加无主书签清理（见 cleanupOrphanBookmarks）
     * @return 清理的记录数
     */
    int cleanupOrphanStates();

    /**
     * @brief cleanupOrphanBookmarks
     * 清理无主书签：同一内容指纹的 operation 记录已全部消失，
     * 且书签挂载的所有路径文件均已不存在（不含可移动设备/网络等
     * 存在性不可靠的路径），则该内容的书签一并清理；
     * 任一挂载路径文件仍存在则保留（待用户打开后按指纹找回）
     * @return 清理的书签记录数
     */
    int cleanupOrphanBookmarks();

    /**
     * @brief flushToDisk
     * 强制将数据库缓存数据刷写到磁盘，确保进程异常终止后数据不丢失
     * 在关键数据（书签、阅读进度）保存后调用
     */
    void flushToDisk();

    /**
     * @brief computeContentHash
     * 计算文件前64KB内容的SHA256哈希
     * @param filePath 文件路径
     * @return 哈希值（十六进制字符串）
     */
    static QString computeContentHash(const QString &filePath);

private:
    Q_DISABLE_COPY(Database)

    static Database *s_instance;

    explicit Database(QObject *parent = nullptr);

    /**
     * @brief prepareOperation
     *准备操作记录表(文档操作)
     * @return
     */
    bool prepareOperation();

    /**
     * @brief prepareBookmark
     * 准备书签表
     * @return
     */
    bool prepareBookmark();

    /**
     * @brief prepareTabGroup
     * 准备标签页组表（V1.2新增）
     * @return
     */
    bool prepareTabGroup();

    /**
     * @brief migrateOperationTable
     * 迁移操作表（增加V1.2新字段）
     * @return
     */
    bool migrateOperationTable();

    /**
     * @brief migrateBookmarkTable
     * 迁移书签表（增加 contentHash 内容指纹列）
     * @return
     */
    bool migrateBookmarkTable();

    /**
     * @brief mergeDuplicateRecords
     * 合并同内容（docId 或 fileSize+contentHash 相同）且源文件已不存在的
     * 其他路径记录到当前路径，作为文件移动/重命名后状态迁移的收尾
     * （目标路径已有同内容记录时 matchOperationByContent 不会被调用，
     * 旧路径残留记录在此清理：阅读状态取 lastOpened 较新者，删除源记录，
     * 旧路径书签并入当前路径）。
     * 注意：源文件仍存在的同内容记录不参与合并 —— 同一份内容在多个路径
     * 同时存在（本地+U盘拷贝、多处复制）时，各路径保持独立阅读状态互不覆盖
     * @param sheet 当前文档（filePath 为合并目标路径）
     * @return 是否发生了合并
     */
    bool mergeDuplicateRecords(DocSheet *sheet);

    QSqlDatabase m_database;

};

#endif // DATABASE_H

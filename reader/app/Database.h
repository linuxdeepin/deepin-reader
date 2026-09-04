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
     * 读取书签（带内容指纹校验：同名但内容不同的文件不会继承旧文件的书签）
     * @param filePath 文件名(唯一标识)
     * @param bookmarks 书签列表
     * @return
     */
    bool readBookmarks(const QString &filePath, QSet<int> &bookmarks);

    /**
     * @brief saveBookmarks
     * 保存书签（同时记录当前文件内容指纹，供读取时校验）
     * @param filePath 文件名(唯一标识)
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
     * 通过文件内容特征（大小+修改时间+内容哈希）匹配已保存的操作记录
     * 用于文档移动或重命名后仍能恢复阅读状态
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
     * 网络文档按超时清理：超过 7 天未打开的记录（含书签）被清除
     * @return 清理的记录数
     */
    int cleanupOrphanStates();

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

    QSqlDatabase m_database;

};

#endif // DATABASE_H

// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Database.h"
#include "Global.h"
#include "stub.h"
#include "DocSheet.h"

#include <QTest>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>

#include <gtest/gtest.h>

//class TestTransaction : public ::testing::Test
//{
//public:
//    TestTransaction(): m_tester(nullptr) {}

//public:
//    virtual void SetUp()
//    {
//        QSqlDatabase database;
//        m_tester = new Transaction(database);
//    }

//    virtual void TearDown()
//    {
//        delete m_tester;
//    }

//protected:
//    Transaction *m_tester;
//};

//TEST_F(TestTransaction, inittest)
//{

//}

//bool QSqlDatabase_commit_stub()
//{
//    return true;
//}

//TEST_F(TestTransaction, testcommit)
//{
//    Stub stub;
//    stub.set(ADDR(QSqlDatabase, commit), QSqlDatabase_commit_stub);
//    m_tester->commit();
//    EXPECT_TRUE(m_tester->m_committed == true);
//}

/********测试Database***********/
class TestDatabase : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    Database *m_tester = nullptr;
};

void TestDatabase::SetUp()
{
    m_tester = Database::instance();
}

void TestDatabase::TearDown()
{
    delete m_tester;
}

static bool ut_sqlquery_next()
{
    return true;
}

static bool ut_sqlquery_exec()
{
    return true;
}
/*************测试用例****************/
TEST_F(TestDatabase, UT_Database_prepareOperation_001)
{
    EXPECT_FALSE(m_tester->prepareOperation());
}

TEST_F(TestDatabase, UT_Database_readOperation_001)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.pdf";
    DocSheet *sheet = new DocSheet(Dr::FileType::PDF, strPath, nullptr);

    Stub s;
    s.set(ADDR(QSqlQuery, next), ut_sqlquery_next);
    EXPECT_TRUE(m_tester->readOperation(sheet));
    EXPECT_TRUE(!m_tester->readOperation(nullptr));

    delete sheet;
}

TEST_F(TestDatabase, UT_Database_saveOperation_001)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.pdf";
    DocSheet *sheet = new DocSheet(Dr::FileType::PDF, strPath, nullptr);

    Stub s;
    s.set((bool (QSqlQuery::*)())ADDR(QSqlQuery, exec), ut_sqlquery_exec);
    EXPECT_TRUE(m_tester->saveOperation(sheet));
    EXPECT_TRUE(!m_tester->saveOperation(nullptr));

    delete sheet;
}

TEST_F(TestDatabase, UT_Database_prepareBookmark_001)
{
    EXPECT_FALSE(m_tester->prepareBookmark());
}

//TEST_F(TestDatabase, UT_Database_readBookmarks_001)
//{
//    QString strPath = UTSOURCEDIR;
//    strPath += "/files/normal.pdf";
//    QSet<int> bookmarks = {0, 1};
//    EXPECT_TRUE(m_tester->readBookmarks(strPath, bookmarks));
//}

TEST_F(TestDatabase, UT_Database_saveBookmarks_001)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.pdf";
    QSet<int> bookmarks = {0, 1};

    Stub s;
    s.set((bool (QSqlQuery::*)())ADDR(QSqlQuery, exec), ut_sqlquery_exec);
    EXPECT_TRUE(m_tester->saveBookmarks(strPath, bookmarks));
}


TEST_F(TestDatabase, UT_Database_prepareTabGroup_001)
{
    QSqlQuery drop(m_tester->m_database);
    drop.exec("DROP TABLE IF EXISTS tabgroup");
    EXPECT_TRUE(m_tester->prepareTabGroup());      // 全新建表
    EXPECT_FALSE(m_tester->prepareTabGroup());     // 表已存在
}

TEST_F(TestDatabase, UT_Database_tabGroup_001)
{
    QSqlQuery drop(m_tester->m_database);
    drop.exec("DROP TABLE IF EXISTS tabgroup");
    ASSERT_TRUE(m_tester->prepareTabGroup());

    EXPECT_TRUE(m_tester->saveTabGroup(0, QStringList() << "/tmp/a.pdf" << "/tmp/b.pdf" << "/tmp/c.pdf", 1));

    int activeIndex = -1;
    QStringList files = m_tester->readTabGroup(0, activeIndex);
    EXPECT_EQ(files.size(), 3);
    EXPECT_EQ(activeIndex, 1);
    EXPECT_TRUE(files.contains("/tmp/b.pdf"));

    // 覆盖旧记录后仅剩 1 条
    EXPECT_TRUE(m_tester->saveTabGroup(0, QStringList() << "/tmp/d.pdf", 0));
    files = m_tester->readTabGroup(0, activeIndex);
    EXPECT_EQ(files.size(), 1);
    EXPECT_EQ(activeIndex, 0);

    EXPECT_TRUE(m_tester->clearTabGroup(0));
    files = m_tester->readTabGroup(0, activeIndex);
    EXPECT_TRUE(files.isEmpty());
}

TEST_F(TestDatabase, UT_Database_cleanupOrphanStates_001)
{
    // 构造一条指向不存在文件的阅读记录（不在 /media、/mnt、/run/media 下）
    QString orphanPath = "/tmp/deepin_reader_ut_orphan_cleanup.pdf";
    QFile::remove(orphanPath);
    DocSheet orphanSheet(Dr::FileType::PDF, orphanPath, nullptr);
    EXPECT_TRUE(m_tester->saveOperation(&orphanSheet));

    int cleaned = m_tester->cleanupOrphanStates();
    EXPECT_GE(cleaned, 1);
    // 孤儿记录已被清理，再次执行返回 0
    EXPECT_EQ(m_tester->cleanupOrphanStates(), 0);
}

// 构造 gvfs 网络路径（isNetworkPath 通过字符串前缀识别，无需真实网络挂载）
static QString ut_network_path(const QString &name)
{
    return QString("/run/user/1000/gvfs/smb-share:server=ut,share=share/%1").arg(name);
}

static qint64 ut_operation_count(Database *db, const QString &filePath)
{
    QSqlQuery query(db->m_database);
    query.prepare("SELECT COUNT(*) FROM operation WHERE filePath = :p");
    query.bindValue(":p", filePath);
    query.exec();
    query.next();
    return query.value(0).toLongLong();
}

static qint64 ut_bookmark_count(Database *db, const QString &filePath)
{
    QSqlQuery query(db->m_database);
    query.prepare("SELECT COUNT(*) FROM bookmark WHERE filePath = :p");
    query.bindValue(":p", filePath);
    query.exec();
    query.next();
    return query.value(0).toLongLong();
}

static qint64 ut_bookmark_hash_count(Database *db, const QString &hash)
{
    QSqlQuery query(db->m_database);
    query.prepare("SELECT COUNT(*) FROM bookmark WHERE contentHash = :h");
    query.bindValue(":h", hash);
    query.exec();
    query.next();
    return query.value(0).toLongLong();
}

TEST_F(TestDatabase, UT_Database_cleanupOrphanStates_002)
{
    // 网络文档超时清理：lastOpened 超过 7 天的记录及书签应被清理
    QString netPath = ut_network_path("expired.pdf");
    DocSheet netSheet(Dr::FileType::PDF, netPath, nullptr);
    ASSERT_TRUE(m_tester->saveOperation(&netSheet));
    QSet<int> bookmarks {1, 5, 9};
    ASSERT_TRUE(m_tester->saveBookmarks(netPath, bookmarks));

    // 手动将 lastOpened 设置为 8 天前
    qint64 oldTime = QDateTime::currentMSecsSinceEpoch() - 8LL * 24 * 60 * 60 * 1000;
    QSqlQuery update(m_tester->m_database);
    update.prepare("UPDATE operation SET lastOpened = :t WHERE filePath = :p");
    update.bindValue(":t", oldTime);
    update.bindValue(":p", netPath);
    ASSERT_TRUE(update.exec());

    int cleaned = m_tester->cleanupOrphanStates();
    EXPECT_GE(cleaned, 1);

    // operation 与 bookmark 记录均已被清理
    EXPECT_EQ(ut_operation_count(m_tester, netPath), 0);
    EXPECT_EQ(ut_bookmark_count(m_tester, netPath), 0);
}

TEST_F(TestDatabase, UT_Database_cleanupOrphanStates_003)
{
    // 网络文档未超时：lastOpened 在 7 天内的记录应保留
    // （测试共用数据库可能存在其他历史孤儿记录，不校验返回值，
    //   仅验证本记录未被清理）
    QString netPath = ut_network_path("fresh.pdf");
    DocSheet netSheet(Dr::FileType::PDF, netPath, nullptr);
    ASSERT_TRUE(m_tester->saveOperation(&netSheet));
    QSet<int> bookmarks {2};
    ASSERT_TRUE(m_tester->saveBookmarks(netPath, bookmarks));

    m_tester->cleanupOrphanStates();

    EXPECT_EQ(ut_operation_count(m_tester, netPath), 1);
    EXPECT_EQ(ut_bookmark_count(m_tester, netPath), 1);
}

TEST_F(TestDatabase, UT_Database_lastOpened_001)
{
    // saveOperation 应写入 lastOpened 时间戳
    QString path = "/tmp/deepin_reader_ut_lastopened.pdf";
    DocSheet sheet(Dr::FileType::PDF, path, nullptr);
    ASSERT_TRUE(m_tester->saveOperation(&sheet));

    QSqlQuery query(m_tester->m_database);
    query.prepare("SELECT lastOpened FROM operation WHERE filePath = :p");
    query.bindValue(":p", path);
    ASSERT_TRUE(query.exec());
    ASSERT_TRUE(query.next());

    qint64 lastOpened = query.value(0).toLongLong();
    qint64 now = QDateTime::currentMSecsSinceEpoch();
    // 时间戳在 1 分钟以内视为有效
    EXPECT_GT(lastOpened, now - 60 * 1000);
    EXPECT_LE(lastOpened, now);
}

TEST_F(TestDatabase, UT_Database_cleanupOrphanStates_004)
{
    // 文件不存在（重命名/移动场景）的清理策略：
    // 带指纹且最近打开 → 保留待内容匹配迁移；带指纹但超 7 天 → 清理；
    // 无指纹 → 维持原立即清理策略
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString doc = dir + "/cleanup_keep.pdf";
    QFile::remove(doc);
    ASSERT_TRUE(QFile::copy(src, doc));
    const QString hash = Database::computeContentHash(doc);
    ASSERT_FALSE(hash.isEmpty());

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    const qint64 days8 = qint64(8) * 24 * 60 * 60 * 1000;
    // 三条记录的路径都指向不存在的文件
    const QString ghostRecent = dir + "/ghost_recent.pdf";
    const QString ghostExpired = dir + "/ghost_expired.pdf";
    const QString ghostLegacy = dir + "/ghost_legacy.pdf";

    QSqlQuery query(m_tester->m_database);
    struct Ghost {
        QString path;
        QString hash;
        qint64 lastOpened;
    };
    const QList<Ghost> ghosts = {
        { ghostRecent, hash, now },
        { ghostExpired, hash, now - days8 },
        { ghostLegacy, QString(), now },
    };
    for (const Ghost &g : ghosts) {
        // 幂等清理历史残留
        query.prepare("DELETE FROM operation WHERE filePath = :p");
        query.bindValue(":p", g.path);
        ASSERT_TRUE(query.exec());
        query.prepare("DELETE FROM bookmark WHERE filePath = :p");
        query.bindValue(":p", g.path);
        ASSERT_TRUE(query.exec());
        // 插入 operation + bookmark 记录
        query.prepare("INSERT INTO operation(filePath, contentHash, lastOpened) VALUES(:p, :h, :t)");
        query.bindValue(":p", g.path);
        query.bindValue(":h", g.hash);
        query.bindValue(":t", g.lastOpened);
        ASSERT_TRUE(query.exec());
        query.prepare("INSERT INTO bookmark(filePath, bookmarkIndex, contentHash) VALUES(:p, 0, :h)");
        query.bindValue(":p", g.path);
        query.bindValue(":h", g.hash);
        ASSERT_TRUE(query.exec());
    }

    m_tester->cleanupOrphanStates();

    // 带指纹 + 最近打开：保留，等待打开新路径时按指纹迁移
    EXPECT_EQ(ut_operation_count(m_tester, ghostRecent), 1);
    EXPECT_EQ(ut_bookmark_count(m_tester, ghostRecent), 1);
    // 带指纹但超 7 天未打开：清理
    EXPECT_EQ(ut_operation_count(m_tester, ghostExpired), 0);
    EXPECT_EQ(ut_bookmark_count(m_tester, ghostExpired), 0);
    // 无指纹旧格式记录：立即清理
    EXPECT_EQ(ut_operation_count(m_tester, ghostLegacy), 0);
    EXPECT_EQ(ut_bookmark_count(m_tester, ghostLegacy), 0);

    // 测试库持久共享：清理指向不存在文件的记录，
    // 避免残留干扰后续用例的内容匹配（match 会选 lastOpened 最新者）
    for (const Ghost &g : ghosts) {
        query.prepare("DELETE FROM operation WHERE filePath = :p");
        query.bindValue(":p", g.path);
        ASSERT_TRUE(query.exec());
        query.prepare("DELETE FROM bookmark WHERE filePath = :p");
        query.bindValue(":p", g.path);
        ASSERT_TRUE(query.exec());
    }

    QFile::remove(doc);
}

TEST_F(TestDatabase, UT_Database_cleanupOrphanBookmarks_001)
{
    // 书签以内容指纹为关联键后的无主书签清理：
    // 1) 同指纹的 operation 记录已全部消失，且挂载路径文件不存在 → 清理；
    // 2) 指纹仍有 operation 记录（文件不存在待迁移）→ 保留；
    // 3) 挂载路径为可移动设备（可能未挂载，存在性不可靠）→ 保留；
    // 4) legacy 无指纹书签不受影响，仍走原有路径清理策略
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    const QString deadPath = dir + "/orphan_bm_dead.pdf";      // 不创建文件
    const QString alivePath = dir + "/orphan_bm_alive.pdf";    // 不创建文件，但指纹有主
    const QString mediaPath = "/media/usb0/orphan_bm_media.pdf";
    const QString legacyPath = dir + "/orphan_bm_legacy.pdf";
    const QString deadHash = "fakehash_bm001_dead";
    const QString aliveHash = "fakehash_bm001_alive";
    const QString mediaHash = "fakehash_bm001_media";

    QSqlQuery query(m_tester->m_database);
    auto cleanOne = [&](const QString &p) {
        query.prepare("DELETE FROM operation WHERE filePath = :p");
        query.bindValue(":p", p);
        ASSERT_TRUE(query.exec());
        query.prepare("DELETE FROM bookmark WHERE filePath = :p");
        query.bindValue(":p", p);
        ASSERT_TRUE(query.exec());
    };
    for (const QString &p : { deadPath, alivePath, legacyPath }) {
        cleanOne(p);
    }
    for (const QString &h : { deadHash, aliveHash, mediaHash }) {
        query.prepare("DELETE FROM bookmark WHERE contentHash = :h");
        query.bindValue(":h", h);
        ASSERT_TRUE(query.exec());
    }

    // 1) 无主指纹 + 文件不存在 → 应清理
    query.prepare("INSERT INTO bookmark(filePath, bookmarkIndex, contentHash) "
                  "VALUES(:p, 0, :h)");
    query.bindValue(":p", deadPath);
    query.bindValue(":h", deadHash);
    ASSERT_TRUE(query.exec());

    // 2) 指纹有主（operation 保留待迁移）→ 书签应保留
    query.prepare("INSERT INTO operation(filePath, contentHash, lastOpened) "
                  "VALUES(:p, :h, :t)");
    query.bindValue(":p", alivePath);
    query.bindValue(":h", aliveHash);
    query.bindValue(":t", QDateTime::currentMSecsSinceEpoch());
    ASSERT_TRUE(query.exec());
    query.prepare("INSERT INTO bookmark(filePath, bookmarkIndex, contentHash) "
                  "VALUES(:p, 1, :h)");
    query.bindValue(":p", alivePath);
    query.bindValue(":h", aliveHash);
    ASSERT_TRUE(query.exec());

    // 3) 可移动设备路径：文件不存在但不可靠 → 保留
    query.prepare("INSERT INTO bookmark(filePath, bookmarkIndex, contentHash) "
                  "VALUES(:p, 2, :h)");
    query.bindValue(":p", mediaPath);
    query.bindValue(":h", mediaHash);
    ASSERT_TRUE(query.exec());

    // 4) legacy：无指纹 operation + 无指纹书签 + 文件不存在 → 原逻辑立即清理
    query.prepare("INSERT INTO operation(filePath, contentHash, lastOpened) "
                  "VALUES(:p, '', :t)");
    query.bindValue(":p", legacyPath);
    query.bindValue(":t", QDateTime::currentMSecsSinceEpoch());
    ASSERT_TRUE(query.exec());
    query.prepare("INSERT INTO bookmark(filePath, bookmarkIndex, contentHash) "
                  "VALUES(:p, 3, '')");
    query.bindValue(":p", legacyPath);
    ASSERT_TRUE(query.exec());

    m_tester->cleanupOrphanStates();

    // 1) 清理
    EXPECT_EQ(ut_bookmark_hash_count(m_tester, deadHash), 0);
    // 2) 保留（书签与待迁移的阅读状态一起等待内容重现）
    EXPECT_EQ(ut_bookmark_hash_count(m_tester, aliveHash), 1);
    EXPECT_EQ(ut_operation_count(m_tester, alivePath), 1);
    // 3) 保留
    EXPECT_EQ(ut_bookmark_hash_count(m_tester, mediaHash), 1);
    // 4) legacy 清理
    EXPECT_EQ(ut_bookmark_count(m_tester, legacyPath), 0);
    EXPECT_EQ(ut_operation_count(m_tester, legacyPath), 0);

    // 尾部清理：避免持久库残留干扰后续用例
    for (const QString &p : { alivePath, legacyPath }) {
        cleanOne(p);
    }
    for (const QString &h : { deadHash, aliveHash, mediaHash }) {
        query.prepare("DELETE FROM bookmark WHERE contentHash = :h");
        query.bindValue(":h", h);
        ASSERT_TRUE(query.exec());
    }
}

TEST_F(TestDatabase, UT_Database_cleanupOrphanBookmarks_002)
{
    // 无主指纹但挂载路径文件仍存在 → 保留
    // （同名覆盖后原文件从回收站恢复的场景：书签隐身保留，打开后按指纹找回）
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString doc = dir + "/orphan_bm_kept.pdf";
    QFile::remove(doc);
    ASSERT_TRUE(QFile::copy(src, doc));
    // 用假指纹保证无主且不与其他用例的真实指纹串扰
    const QString fakeHash = "fakehash_bm002_kept";

    QSqlQuery query(m_tester->m_database);
    query.prepare("DELETE FROM operation WHERE filePath = :p");
    query.bindValue(":p", doc);
    ASSERT_TRUE(query.exec());
    query.prepare("DELETE FROM bookmark WHERE filePath = :p");
    query.bindValue(":p", doc);
    ASSERT_TRUE(query.exec());
    query.prepare("DELETE FROM bookmark WHERE contentHash = :h");
    query.bindValue(":h", fakeHash);
    ASSERT_TRUE(query.exec());

    // 文件存在但没有 operation 记录（同名覆盖后残留的旧内容书签）
    query.prepare("INSERT INTO bookmark(filePath, bookmarkIndex, contentHash) "
                  "VALUES(:p, 4, :h)");
    query.bindValue(":p", doc);
    query.bindValue(":h", fakeHash);
    ASSERT_TRUE(query.exec());

    m_tester->cleanupOrphanStates();

    // 挂载路径文件存在 → 无主书签保留
    EXPECT_EQ(ut_bookmark_hash_count(m_tester, fakeHash), 1);

    // 尾部清理
    query.prepare("DELETE FROM bookmark WHERE contentHash = :h");
    query.bindValue(":h", fakeHash);
    ASSERT_TRUE(query.exec());
    query.prepare("DELETE FROM bookmark WHERE filePath = :p");
    query.bindValue(":p", doc);
    ASSERT_TRUE(query.exec());
    QFile::remove(doc);
}

TEST_F(TestDatabase, UT_Database_renameBookmarkMigration_001)
{
    // 端到端：文件重命名 → 启动清理不再误删旧记录 →
    // 打开新路径时 matchOperationByContent 迁移阅读状态与书签
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString doc1 = dir + "/rename_src.pdf";
    const QString docMoved = dir + "/rename_dst.pdf";

    // 清理历史残留
    DocSheet sheet(Dr::FileType::PDF, doc1, nullptr);
    m_tester->saveBookmarks(doc1, QSet<int>());
    m_tester->saveBookmarks(docMoved, QSet<int>());

    ASSERT_TRUE(QFile::copy(src, doc1));

    // 文档加书签并保存阅读状态（saveOperation 同时写入指纹）
    ASSERT_TRUE(m_tester->saveBookmarks(doc1, QSet<int> {0}));
    sheet.m_operation.currentPage = 5;
    ASSERT_TRUE(m_tester->saveOperation(&sheet, Database::computeContentHash(doc1)));

    // 用户重命名文件（内容不变）
    ASSERT_TRUE(QFile::rename(doc1, docMoved));

    // 模拟下次启动的孤立记录清理：旧路径记录应保留（带指纹、最近打开）
    m_tester->cleanupOrphanStates();
    EXPECT_EQ(ut_operation_count(m_tester, doc1), 1);
    EXPECT_EQ(ut_bookmark_count(m_tester, doc1), 1);

    // 打开新路径：内容匹配迁移阅读状态；书签随迁移一并转到新路径
    EXPECT_TRUE(m_tester->matchOperationByContent(QFileInfo(docMoved), &sheet));
    EXPECT_EQ(ut_operation_count(m_tester, doc1), 0);
    EXPECT_EQ(ut_bookmark_count(m_tester, doc1), 0);
    EXPECT_EQ(ut_bookmark_count(m_tester, docMoved), 1);

    QSet<int> bookmarks;
    EXPECT_TRUE(m_tester->readBookmarks(docMoved, bookmarks));
    EXPECT_EQ(bookmarks, QSet<int> {0});

    // 清理迁移后的记录
    m_tester->saveBookmarks(docMoved, QSet<int>());
    QSqlQuery clean(m_tester->m_database);
    clean.prepare("DELETE FROM operation WHERE filePath = :p");
    clean.bindValue(":p", docMoved);
    ASSERT_TRUE(clean.exec());

    QFile::remove(docMoved);
}

// ===== 书签内容指纹校验 =====

// 按内容指纹清理书签记录（书签跨路径读取，固定路径的用例间会通过同 hash 内容互相污染）
static void ut_cleanup_content(Database *db, const QString &hash)
{
    QSqlQuery query(db->m_database);
    query.prepare("DELETE FROM bookmark WHERE contentHash = :h");
    query.bindValue(":h", hash);
    ASSERT_TRUE(query.exec());
}

// 清理指定路径的 operation/bookmark 记录（测试库持久共享，避免用例间串扰）
static void ut_cleanup_path(Database *db, const QString &path)
{
    QSqlQuery query(db->m_database);
    query.prepare("DELETE FROM operation WHERE filePath = :p");
    query.bindValue(":p", path);
    ASSERT_TRUE(query.exec());
    query.prepare("DELETE FROM bookmark WHERE filePath = :p");
    query.bindValue(":p", path);
    ASSERT_TRUE(query.exec());
}

// 准备临时目录及两个内容不同的真实文件（computeContentHash 需要读取文件）
static QString ut_bookmark_prepare_files(QString &doc1, QString &doc2)
{
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    doc1 = dir + "/doc.pdf";
    doc2 = dir + "/doc2.pdf";
    QFile::remove(doc1);
    QFile::remove(doc2);
    EXPECT_TRUE(QFile::copy(src, doc1));
    EXPECT_TRUE(QFile::copy(src, doc2));
    // 追加数据使 doc2 与 doc1 内容不同
    QFile f2(doc2);
    EXPECT_TRUE(f2.open(QIODevice::Append));
    f2.write(QByteArray(4096, 'x'));
    f2.close();
    return dir;
}

TEST_F(TestDatabase, UT_Database_bookmarkContentHash_001)
{
    // 同名不同内容的文件不应继承旧文件的书签
    // （bug场景：文档1加书签后删除，文档2改名为文档1，文档2不应显示书签）
    QString doc1, doc2;
    ut_bookmark_prepare_files(doc1, doc2);
    ASSERT_NE(Database::computeContentHash(doc1), Database::computeContentHash(doc2));

    // 测试库持久共享：先清掉同指纹历史残留（书签跨路径读取，会互相污染）
    ut_cleanup_content(m_tester, Database::computeContentHash(doc1));

    // 文档1 第 1 页添加书签，正常读取
    ASSERT_TRUE(m_tester->saveBookmarks(doc1, QSet<int> {0}));
    QSet<int> read;
    EXPECT_TRUE(m_tester->readBookmarks(doc1, read));
    EXPECT_EQ(read, QSet<int> {0});

    // 文档1 被删除，文档2 改名为文档1（同路径、不同内容）
    QFile::remove(doc1);
    ASSERT_TRUE(QFile::rename(doc2, doc1));

    // 旧文件的书签不应出现在新文件上；记录按指纹保留在库中（隐身），
    // 原内容恢复后书签可自动找回
    read.clear();
    EXPECT_TRUE(m_tester->readBookmarks(doc1, read));
    EXPECT_TRUE(read.isEmpty());
    EXPECT_EQ(ut_bookmark_count(m_tester, doc1), 1);

    // 清理残留（doc1 现为 doc2 内容，其指纹无书签记录，清理 doc1 路径即可）
    ut_cleanup_path(m_tester, doc1);
    QFile::remove(doc1);
}

TEST_F(TestDatabase, UT_Database_readOperationContentHash_001)
{
    // 同名不同内容的文件不应继承旧文档的阅读状态（与书签同根因）
    QString doc1, doc2;
    ut_bookmark_prepare_files(doc1, doc2);

    // 文档1 正常阅读并保存状态（此时文件内容为 H1）
    {
        DocSheet sheet1(Dr::FileType::PDF, doc1, nullptr);
        sheet1.m_operation.currentPage = 42;
        ASSERT_TRUE(m_tester->saveOperation(&sheet1));
    }
    // 注意：sheet1 析构（setAlive(false)）会再次 saveOperation，记录内容不变

    // 文档1 被删除，文档2 改名为文档1（同路径、不同内容）
    QFile::remove(doc1);
    ASSERT_TRUE(QFile::rename(doc2, doc1));

    // 打开新文件：过期状态不应被恢复，且过期记录已被清除
    // （DocSheet 构造即 setAlive(true)，此处 readOperation 已被调用过一次并删除过期记录）
    {
        DocSheet sheet2(Dr::FileType::PDF, doc1, nullptr);
        EXPECT_FALSE(m_tester->readOperation(&sheet2));
        // currentPage 保持默认值 1，未被旧文档状态（42）污染
        EXPECT_EQ(sheet2.m_operation.currentPage, 1);
        EXPECT_EQ(ut_operation_count(m_tester, doc1), 0);
    }

    QFile::remove(doc1);
}

TEST_F(TestDatabase, UT_Database_readOperationContentHash_002)
{
    // 同一文件状态正常恢复；旧版本记录（无指纹）读取时回填
    // （使用独立路径，避免其它用例 DocSheet 析构时 saveOperation 写入的记录干扰）
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString doc = dir + "/legacy_op.pdf";
    QFile::remove(doc);
    ASSERT_TRUE(QFile::copy(src, doc));

    // 幂等清理：DocSheet 析构（setAlive(false)）会 saveOperation 写回记录，
    // 测试数据库持久化，需先清除历史残留避免主键冲突
    QSqlQuery clean(m_tester->m_database);
    clean.prepare("DELETE FROM operation WHERE filePath = :p");
    clean.bindValue(":p", doc);
    ASSERT_TRUE(clean.exec());
    qWarning() << "DELETE affected:" << clean.numRowsAffected();

    // 直接插入一条无指纹的旧格式记录（exec 仅执行一次，重复执行会撞主键）
    QSqlQuery insert(m_tester->m_database);
    insert.prepare("INSERT INTO operation(filePath, currentPage, contentHash) VALUES(:p, 7, '')");
    insert.bindValue(":p", doc);
    const bool inserted = insert.exec();
    if (!inserted) {
        qWarning() << "INSERT failed:" << insert.lastError().text();
    }
    ASSERT_TRUE(inserted);

    {
        DocSheet sheet(Dr::FileType::PDF, doc, nullptr);
        EXPECT_TRUE(m_tester->readOperation(&sheet));
        EXPECT_EQ(sheet.m_operation.currentPage, 7);

        // 指纹已回填，再次读取仍能恢复
        QSqlQuery query(m_tester->m_database);
        query.prepare("SELECT contentHash FROM operation WHERE filePath = :p");
        query.bindValue(":p", doc);
        ASSERT_TRUE(query.exec());
        ASSERT_TRUE(query.next());
        EXPECT_EQ(query.value(0).toString(), Database::computeContentHash(doc));

        EXPECT_TRUE(m_tester->readOperation(&sheet));
        EXPECT_EQ(sheet.m_operation.currentPage, 7);
    }
    // sheet 已析构，清理其写回的记录
    ASSERT_TRUE(clean.exec());

    QFile::remove(doc);
}

TEST_F(TestDatabase, UT_Database_bookmarkContentHash_002)
{
    // 同一文件书签读写不受影响；旧版本数据（无指纹）读取时回填
    QString doc1, doc2;
    ut_bookmark_prepare_files(doc1, doc2);
    QFile::remove(doc2);

    // 测试库持久共享：清理同指纹历史残留
    ut_cleanup_content(m_tester, Database::computeContentHash(doc1));

    // 直接插入一条无指纹的旧格式记录
    QSqlQuery insert(m_tester->m_database);
    insert.prepare("INSERT INTO bookmark(filePath, bookmarkIndex, contentHash) VALUES(:p, :i, '')");
    insert.bindValue(":p", doc1);
    insert.bindValue(":i", 3);
    ASSERT_TRUE(insert.exec());

    // 读取时旧书签保留且指纹被回填
    QSet<int> read;
    EXPECT_TRUE(m_tester->readBookmarks(doc1, read));
    EXPECT_EQ(read, QSet<int> {3});

    QSqlQuery query(m_tester->m_database);
    query.prepare("SELECT contentHash FROM bookmark WHERE filePath = :p");
    query.bindValue(":p", doc1);
    ASSERT_TRUE(query.exec());
    ASSERT_TRUE(query.next());
    EXPECT_EQ(query.value(0).toString(), Database::computeContentHash(doc1));

    // 同一文件再次读取，书签仍正常
    read.clear();
    EXPECT_TRUE(m_tester->readBookmarks(doc1, read));
    EXPECT_EQ(read, QSet<int> {3});

    ut_cleanup_content(m_tester, Database::computeContentHash(doc1));
    QFile::remove(doc1);
}

TEST_F(TestDatabase, UT_Database_bookmarkContentHash_003)
{
    // 文件不可读（无法计算指纹）时跳过校验，按旧行为返回书签且不删除记录
    QString doc1, doc2;
    ut_bookmark_prepare_files(doc1, doc2);
    QFile::remove(doc2);

    // 测试库持久共享：清理同指纹历史残留
    ut_cleanup_content(m_tester, Database::computeContentHash(doc1));

    ASSERT_TRUE(m_tester->saveBookmarks(doc1, QSet<int> {2}));

    // 移除读权限，使 computeContentHash 失败（root 下仍可读，不走该分支，但用例仍应通过）
    QFile::setPermissions(doc1, QFile::Permissions());

    QSet<int> read;
    EXPECT_TRUE(m_tester->readBookmarks(doc1, read));
    EXPECT_EQ(read, QSet<int> {2});
    EXPECT_EQ(ut_bookmark_count(m_tester, doc1), 1);

    QFile::setPermissions(doc1, QFile::ReadOwner | QFile::WriteOwner);
    // 清理残留：无权限期间 saveBookmarks 按路径删除并写入无指纹记录
    ut_cleanup_path(m_tester, doc1);
    ut_cleanup_content(m_tester, Database::computeContentHash(doc1));
    QFile::remove(doc1);
}

// ===== 同内容多路径记录合并（文件移动/复制到已存在同名旧记录的路径） =====

// 以 SQL 直写一条 operation 记录。
// 注意：不要用 DocSheet 构造来造记录 —— 其构造即 setAlive(true)，
// 会隐式触发 readOperation/matchOperationByContent 提前迁移同内容记录，污染场景
static void ut_insert_operation(Database *db, const QString &path, int currentPage,
                                const QString &hash, qint64 lastOpened)
{
    // fileSize 必须写真实值：matchOperationByContent / mergeDuplicateRecords
    // 均按 fileSize+contentHash 匹配，写默认值 0 会导致匹配不上
    QSqlQuery insert(db->m_database);
    insert.prepare("INSERT INTO operation(filePath, currentPage, contentHash, lastOpened, fileSize) "
                   "VALUES(:p, :c, :h, :t, :s)");
    insert.bindValue(":p", path);
    insert.bindValue(":c", currentPage);
    insert.bindValue(":h", hash);
    insert.bindValue(":t", lastOpened);
    insert.bindValue(":s", QFileInfo(path).size());
    ASSERT_TRUE(insert.exec());
}

TEST_F(TestDatabase, UT_Database_mergeDuplicateRecords_001)
{
    // bug 场景：本地与U盘存在同一份文档（内容相同），U盘路径已有旧记录（无书签），
    // 本地文档加书签/进度后移动到U盘替换，重新打开U盘文档时
    // readOperation 命中U盘旧记录且指纹校验通过，应触发同内容记录合并：
    // 书签迁移、进度恢复为带书签记录（本地）的状态、旧路径记录清理
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString docLocal = dir + "/merge_local.pdf";
    const QString docUsb = dir + "/merge_usb.pdf";
    QFile::remove(docLocal);
    QFile::remove(docUsb);
    ASSERT_TRUE(QFile::copy(src, docLocal));
    ASSERT_TRUE(QFile::copy(src, docUsb));
    const QString hash = Database::computeContentHash(docLocal);
    ASSERT_EQ(hash, Database::computeContentHash(docUsb));

    // 幂等清理历史残留（残留记录会干扰内容匹配）
    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);

    // U盘文档曾打开过，留下旧记录（currentPage=2，无书签）
    {
        DocSheet sheetUsb(Dr::FileType::PDF, docUsb, nullptr);
        sheetUsb.m_operation.currentPage = 2;
        ASSERT_TRUE(m_tester->saveOperation(&sheetUsb, hash));
    }

    // 本地文档：阅读到第3页、书签{0,2,4}，正常关闭落库（SQL 直写）
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    ut_insert_operation(m_tester, docLocal, 3, hash, now);
    ASSERT_TRUE(m_tester->saveBookmarks(docLocal, QSet<int> {0, 2, 4}));

    // 文件移动到U盘替换（内容不变）：本地源文件已不存在，
    // 重新打开U盘文档才会触发同内容记录合并（源文件仍存在时各自独立）
    ASSERT_TRUE(QFile::remove(docLocal));

    // readOperation 命中U盘旧记录，合并后应恢复本地文档的状态（带书签的记录优先）
    {
        DocSheet sheetUsbAgain(Dr::FileType::PDF, docUsb, nullptr);
        EXPECT_TRUE(m_tester->readOperation(&sheetUsbAgain));
        EXPECT_EQ(sheetUsbAgain.m_operation.currentPage, 3);
    }

    // 书签随合并迁移到U盘路径（源文件已不存在，书签跟文档走）
    QSet<int> bookmarks;
    EXPECT_TRUE(m_tester->readBookmarks(docUsb, bookmarks));
    EXPECT_EQ(bookmarks, (QSet<int> {0, 2, 4}));
    // 书签已并入U盘路径，本地路径不再残留书签与 operation 记录
    EXPECT_EQ(ut_bookmark_count(m_tester, docLocal), 0);
    EXPECT_EQ(ut_bookmark_count(m_tester, docUsb), 3);
    EXPECT_EQ(ut_operation_count(m_tester, docLocal), 0);
    EXPECT_EQ(ut_operation_count(m_tester, docUsb), 1);

    ut_cleanup_path(m_tester, docUsb);
    QFile::remove(docLocal);
    QFile::remove(docUsb);
}

TEST_F(TestDatabase, UT_Database_mergeDuplicateRecords_002)
{
    // 两边都有书签：书签并集去重，状态取 lastOpened 较新者
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString docLocal = dir + "/merge2_local.pdf";
    const QString docUsb = dir + "/merge2_usb.pdf";
    QFile::remove(docLocal);
    QFile::remove(docUsb);
    ASSERT_TRUE(QFile::copy(src, docLocal));
    ASSERT_TRUE(QFile::copy(src, docUsb));
    const QString hash = Database::computeContentHash(docLocal);

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    // U盘旧记录：书签{0,2}，lastOpened 较旧
    ut_insert_operation(m_tester, docUsb, 2, hash, now - 60000);
    ASSERT_TRUE(m_tester->saveBookmarks(docUsb, QSet<int> {0, 2}));
    // 本地记录：lastOpened 较新；书签按路径独立，本地会话初始不继承U盘书签，
    // 会话中自行添加 {0,2,4}，保存时写入本地路径
    ut_insert_operation(m_tester, docLocal, 7, hash, now);
    QSet<int> localSession;
    EXPECT_TRUE(m_tester->readBookmarks(docLocal, localSession));
    EXPECT_TRUE(localSession.isEmpty());
    ASSERT_TRUE(m_tester->saveBookmarks(docLocal, QSet<int> {0, 2, 4}));

    // 文件移动场景：本地源文件已不存在（源文件仍存在时各自独立，不合并）
    ASSERT_TRUE(QFile::remove(docLocal));

    {
        DocSheet sheetUsbAgain(Dr::FileType::PDF, docUsb, nullptr);
        EXPECT_TRUE(m_tester->readOperation(&sheetUsbAgain));
        // 两边都有书签 → lastOpened 较新的 local 优先
        EXPECT_EQ(sheetUsbAgain.m_operation.currentPage, 7);
    }

    // 书签按内容指纹共享：并集 {0,2,4}，无重复页
    QSet<int> bookmarks;
    EXPECT_TRUE(m_tester->readBookmarks(docUsb, bookmarks));
    EXPECT_EQ(bookmarks, (QSet<int> {0, 2, 4}));
    EXPECT_EQ(ut_operation_count(m_tester, docLocal), 0);

    ut_cleanup_path(m_tester, docUsb);
    QFile::remove(docLocal);
    QFile::remove(docUsb);
}

TEST_F(TestDatabase, UT_Database_mergeDuplicateRecords_003)
{
    // 都无书签：取 lastOpened 较新者；同时验证 docId 匹配分支（hash 不同、docId 相同）
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString docLocal = dir + "/merge3_local.pdf";
    const QString docUsb = dir + "/merge3_usb.pdf";
    QFile::remove(docLocal);
    QFile::remove(docUsb);
    ASSERT_TRUE(QFile::copy(src, docLocal));
    // 追加数据使 hash 不同，但 docId 相同（模拟同一文档的不同修改版本）
    ASSERT_TRUE(QFile::copy(src, docUsb));
    {
        QFile f(docUsb);
        ASSERT_TRUE(f.open(QIODevice::Append));
        f.write(QByteArray(4096, 'y'));
    }
    const QString hashUsb = Database::computeContentHash(docUsb);
    const QString hashLocal = Database::computeContentHash(docLocal);
    ASSERT_NE(hashUsb, hashLocal);

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    // U盘记录：较新（currentPage=9）
    ut_insert_operation(m_tester, docUsb, 9, hashUsb, now);
    // 本地记录：较旧（currentPage=3），docId 与U盘记录相同
    ut_insert_operation(m_tester, docLocal, 3, hashLocal, now - 60000);
    QSqlQuery setDocId(m_tester->m_database);
    ASSERT_TRUE(setDocId.exec("UPDATE operation SET docId = 'ut-merge-docid' "
                              "WHERE filePath IN ('" + docUsb + "', '" + docLocal + "')"));
    ASSERT_EQ(setDocId.numRowsAffected(), 2);

    // 文件移动场景：本地源文件已不存在（源文件仍存在时各自独立，不合并）
    ASSERT_TRUE(QFile::remove(docLocal));

    {
        DocSheet sheetUsbAgain(Dr::FileType::PDF, docUsb, nullptr);
        EXPECT_TRUE(m_tester->readOperation(&sheetUsbAgain));
        // docId 匹配到 local 记录，但其 lastOpened 较旧且无书签 → 状态保持U盘自己的（较新）
        EXPECT_EQ(sheetUsbAgain.m_operation.currentPage, 9);
    }

    // 同 docId 的旧路径记录已清理
    EXPECT_EQ(ut_operation_count(m_tester, docLocal), 0);

    ut_cleanup_path(m_tester, docUsb);
    QFile::remove(docLocal);
    QFile::remove(docUsb);
}

TEST_F(TestDatabase, UT_Database_matchContentUsbExisting_001)
{
    // matchOperationByContent 目标路径已有旧记录（同名替换场景）：
    // 迁移 UPDATE 不应再因 operation.filePath 主键冲突而失败，
    // 且目标路径/源路径下与当前内容不符的遗留旧书签不应被迁移
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString docLocal = dir + "/mc_local.pdf";
    const QString docUsb = dir + "/mc_usb.pdf";
    QFile::remove(docLocal);
    QFile::remove(docUsb);
    ASSERT_TRUE(QFile::copy(src, docLocal));
    ASSERT_TRUE(QFile::copy(src, docUsb));
    const QString hash = Database::computeContentHash(docLocal);

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);

    // 先构造 sheet（此时库中无记录，构造时的隐式匹配不会迁移任何数据）
    DocSheet sheet(Dr::FileType::PDF, docUsb, nullptr);

    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    // U盘旧记录：无书签，另遗留一份与当前内容不符的旧书签（模拟旧文件残留）
    ut_insert_operation(m_tester, docUsb, 2, hash, now - 60000);
    QSqlQuery staleBm(m_tester->m_database);
    staleBm.prepare("INSERT INTO bookmark(filePath, bookmarkIndex, contentHash) VALUES(:p, 8, 'stale-hash')");
    staleBm.bindValue(":p", docUsb);
    ASSERT_TRUE(staleBm.exec());

    // 本地记录：书签+进度
    ut_insert_operation(m_tester, docLocal, 3, hash, now);
    ASSERT_TRUE(m_tester->saveBookmarks(docLocal, QSet<int> {0, 2, 4}));

    // 文件移动场景：本地源文件已不存在，迁移才会发生（仍存在时不夺走其记录）
    ASSERT_TRUE(QFile::remove(docLocal));

    // 直接触发内容匹配迁移（模拟 readOperation miss 后的兜底路径）
    EXPECT_TRUE(m_tester->matchOperationByContent(QFileInfo(docUsb), &sheet));
    EXPECT_EQ(sheet.m_operation.currentPage, 3);

    // 旧路径记录清理；书签随内容匹配迁移到新路径，旧 hash 脏书签读不出来
    QSet<int> bookmarks;
    EXPECT_TRUE(m_tester->readBookmarks(docUsb, bookmarks));
    EXPECT_EQ(bookmarks, (QSet<int> {0, 2, 4}));
    EXPECT_EQ(ut_operation_count(m_tester, docLocal), 0);
    EXPECT_EQ(ut_bookmark_count(m_tester, docLocal), 0);
    // U盘路径：迁移来的 3 条 + 遗留的 1 条 stale-hash 脏书签（读不出来）
    EXPECT_EQ(ut_bookmark_count(m_tester, docUsb), 4);

    ut_cleanup_path(m_tester, docUsb);
    QFile::remove(docLocal);
    QFile::remove(docUsb);
}

// ===== 同内容多路径共存：各路径保持独立阅读状态 =====
// 场景：本地与U盘（均已挂载）各有一份相同内容的文档，两个路径的记录
// 互不合并/迁移：打开任一副本读到各自的状态，另一方记录不受影响；
// 同内容新路径也不得夺走现存路径的记录。
// （修复前：mergeDuplicateRecords 无条件合并并删除对方记录，导致
//  两个文档缩放互相覆盖、关闭后只剩一条记录）
TEST_F(TestDatabase, UT_Database_sameContentCoexistIndependence_001)
{
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString docLocal = dir + "/coexist_local.pdf";
    const QString docUsb = dir + "/coexist_usb.pdf";
    QFile::remove(docLocal);
    QFile::remove(docUsb);
    ASSERT_TRUE(QFile::copy(src, docLocal));
    ASSERT_TRUE(QFile::copy(src, docUsb));
    const QString hash = Database::computeContentHash(docLocal);
    ASSERT_EQ(hash, Database::computeContentHash(docUsb));

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);

    // 两个路径各自的阅读状态：页码/缩放均不同
    const qint64 now = QDateTime::currentMSecsSinceEpoch();
    ut_insert_operation(m_tester, docLocal, 3, hash, now - 60000);
    ut_insert_operation(m_tester, docUsb, 9, hash, now);
    QSqlQuery scale(m_tester->m_database);
    scale.prepare("UPDATE operation SET scaleFactor = 2.0 WHERE filePath = :p");
    scale.bindValue(":p", docLocal);
    ASSERT_TRUE(scale.exec());

    // 打开本地副本：读到本地自己的状态（zoom 2.0 / page 3），
    // 不被U盘记录（较新）覆盖，U盘记录也不被合并删除
    {
        DocSheet sheetLocal(Dr::FileType::PDF, docLocal, nullptr);
        EXPECT_TRUE(m_tester->readOperation(&sheetLocal));
        EXPECT_EQ(sheetLocal.m_operation.currentPage, 3);
        EXPECT_EQ(sheetLocal.m_operation.scaleFactor, 2.0);
        EXPECT_EQ(ut_operation_count(m_tester, docUsb), 1);
    }

    // 打开U盘副本：读到U盘自己的状态（page 9），本地记录保留
    {
        DocSheet sheetUsb(Dr::FileType::PDF, docUsb, nullptr);
        EXPECT_TRUE(m_tester->readOperation(&sheetUsb));
        EXPECT_EQ(sheetUsb.m_operation.currentPage, 9);
        EXPECT_EQ(sheetUsb.m_operation.scaleFactor, 0.1);  // 记录未写缩放，读出钳位默认值，非本地的 2.0
        EXPECT_EQ(ut_operation_count(m_tester, docLocal), 1);
    }

    // 第三个路径首开同内容文件（自身无记录）：源文件均仍存在 → 首开借用。
    // 借用最近修改的源记录（docUsb）的状态，书签复制一份到新路径，
    // 但不得夺走任一现存路径的记录（不迁移、不新建 operation 行）
    {
        const QString docThird = dir + "/coexist_third.pdf";
        QFile::remove(docThird);
        ASSERT_TRUE(QFile::copy(src, docThird));
        ut_cleanup_path(m_tester, docThird);
        ASSERT_TRUE(m_tester->saveBookmarks(docUsb, QSet<int> {1, 4}));
        QSqlQuery touch(m_tester->m_database);
        touch.prepare("UPDATE operation SET lastModified = :t WHERE filePath = :p");
        touch.bindValue(":t", QDateTime::currentMSecsSinceEpoch());
        touch.bindValue(":p", docUsb);
        ASSERT_TRUE(touch.exec());

        DocSheet sheetThird(Dr::FileType::PDF, docThird, nullptr);
        EXPECT_TRUE(m_tester->matchOperationByContent(QFileInfo(docThird), &sheetThird));
        // 借用 docUsb 的状态（page 9，缩放未写读出钳位 0.1），非 docLocal（page 3 / zoom 2.0）
        EXPECT_EQ(sheetThird.m_operation.currentPage, 9);
        EXPECT_EQ(sheetThird.m_operation.scaleFactor, 0.1);
        // 现存路径记录不被夺走，借用也不创建新 operation 记录
        EXPECT_EQ(ut_operation_count(m_tester, docLocal), 1);
        EXPECT_EQ(ut_operation_count(m_tester, docUsb), 1);
        EXPECT_EQ(ut_operation_count(m_tester, docThird), 0);
        // 书签已复制到新路径，源路径书签原样保留
        EXPECT_EQ(ut_bookmark_count(m_tester, docThird), 2);
        EXPECT_EQ(ut_bookmark_count(m_tester, docUsb), 2);
        ut_cleanup_path(m_tester, docThird);
        QFile::remove(docThird);
    }

    // 两个副本各自保存关闭后：两条记录仍在，互不覆盖
    EXPECT_EQ(ut_operation_count(m_tester, docLocal), 1);
    EXPECT_EQ(ut_operation_count(m_tester, docUsb), 1);

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);
    QFile::remove(docLocal);
    QFile::remove(docUsb);
}

// QA 用例[2]：本地阅读后关闭（展开缩略图、自定义最小宽度、第4页、加书签），
// 复制文档到 U 盘后从 U 盘首开：状态与书签应与源文档一致（首开借用）；
// U 盘关闭落盘自己的记录后，两路径各自独立互不覆盖
TEST_F(TestDatabase, UT_Database_borrowStateOnFirstOpen_001)
{
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString docLocal = dir + "/borrow_local.pdf";
    const QString docUsb = dir + "/borrow_usb.pdf";
    QFile::remove(docLocal);
    QFile::remove(docUsb);
    ASSERT_TRUE(QFile::copy(src, docLocal));
    ASSERT_TRUE(QFile::copy(src, docUsb));
    const QString hash = Database::computeContentHash(docLocal);
    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);

    // 本地阅读状态：展开缩略图（sidebarIndex=1）、自定义宽度 120、
    // 第 4 页、滚动位置 0.25、书签 {3}
    ut_insert_operation(m_tester, docLocal, 4, hash, QDateTime::currentMSecsSinceEpoch());
    QSqlQuery state(m_tester->m_database);
    state.prepare("UPDATE operation SET sidebarVisible = 1, sidebarIndex = 1, "
                  "sidebarWidth = 120, sidebarWidthChanged = 1, scrollPosition = 0.25, "
                  "lastModified = :t WHERE filePath = :p");
    state.bindValue(":t", QDateTime::currentMSecsSinceEpoch());
    state.bindValue(":p", docLocal);
    ASSERT_TRUE(state.exec());
    ASSERT_TRUE(m_tester->saveBookmarks(docLocal, QSet<int> {3}));

    // U 盘副本首开：自身无记录 → readOperation miss，matchOperationByContent 借用
    {
        DocSheet sheetUsb(Dr::FileType::PDF, docUsb, nullptr);
        EXPECT_FALSE(m_tester->readOperation(&sheetUsb));
        EXPECT_TRUE(m_tester->matchOperationByContent(QFileInfo(docUsb), &sheetUsb));
        // 状态与源一致：第 4 页、缩略图面板可见、宽度 120、滚动位置
        EXPECT_EQ(sheetUsb.m_operation.currentPage, 4);
        EXPECT_EQ(sheetUsb.m_operation.sidebarVisible, 1);
        EXPECT_EQ(sheetUsb.m_operation.sidebarIndex, 1);
        EXPECT_EQ(sheetUsb.m_operation.sidebarWidth, 120);
        EXPECT_TRUE(sheetUsb.m_operation.sidebarWidthChanged);
        EXPECT_FLOAT_EQ(sheetUsb.m_operation.scrollPosition, 0.25f);
        // 源记录未被夺走，借用不新建 operation 记录
        EXPECT_EQ(ut_operation_count(m_tester, docLocal), 1);
        EXPECT_EQ(ut_operation_count(m_tester, docUsb), 0);
        // 书签已复制到新路径（setAlive 后续 readBookmarks 可直接读到），源路径保留
        QSet<int> borrowed;
        EXPECT_TRUE(m_tester->readBookmarks(docUsb, borrowed));
        EXPECT_EQ(borrowed, QSet<int> {3});
        EXPECT_EQ(ut_bookmark_count(m_tester, docLocal), 1);

        // 模拟关闭落盘：U 盘副本写入自己的记录
        ASSERT_TRUE(m_tester->saveOperation(&sheetUsb, hash));
    }

    // 二次打开：读自己的记录（不再借用）；本地后续变化不串扰 U 盘副本
    QSqlQuery localMove(m_tester->m_database);
    localMove.prepare("UPDATE operation SET currentPage = 6 WHERE filePath = :p");
    localMove.bindValue(":p", docLocal);
    ASSERT_TRUE(localMove.exec());
    ASSERT_TRUE(m_tester->saveBookmarks(docUsb, QSet<int> {3, 5}));
    {
        DocSheet sheetUsb2(Dr::FileType::PDF, docUsb, nullptr);
        EXPECT_TRUE(m_tester->readOperation(&sheetUsb2));
        EXPECT_EQ(sheetUsb2.m_operation.currentPage, 4);
        QSet<int> bms;
        EXPECT_TRUE(m_tester->readBookmarks(docUsb, bms));
        EXPECT_EQ(bms, (QSet<int> {3, 5}));
        EXPECT_EQ(ut_bookmark_count(m_tester, docLocal), 1);
    }

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);
    QFile::remove(docLocal);
    QFile::remove(docUsb);
}

TEST_F(TestDatabase, UT_Database_bookmarkForkContentHash_001)
{
    // 内容分叉场景（BUG-376029 衍生）：同一文档的两份拷贝，其中一份保存高亮注释后
    // 内容指纹变化，两份文件的书签都应保留：
    // - 注释后的文件（新指纹）：书签继续可用
    // - 原始文件（旧指纹）：书签不被删除，再打开仍可找回
    // （修复前：saveBookmarks 按路径删除 + readBookmarks 按 stale 清理，旧指纹书签全丢）
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString docLocal = dir + "/fork_local.pdf";
    const QString docUsb = dir + "/fork_usb.pdf";
    QFile::remove(docLocal);
    QFile::remove(docUsb);
    ASSERT_TRUE(QFile::copy(src, docLocal));
    ASSERT_TRUE(QFile::copy(src, docUsb));
    const QString hash0 = Database::computeContentHash(docLocal);
    ASSERT_EQ(hash0, Database::computeContentHash(docUsb));

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);

    // 本地文档加书签
    ASSERT_TRUE(m_tester->saveBookmarks(docLocal, QSet<int> {0, 2, 4}));

    // 打开U盘文档（内容仍为 hash0）：书签按路径独立，不继承本地路径的书签
    QSet<int> read;
    EXPECT_TRUE(m_tester->readBookmarks(docUsb, read));
    EXPECT_TRUE(read.isEmpty());

    // 在U盘文档上加高亮注释并保存 → 文件内容变化（指纹变为 hash1）
    {
        QFile f(docUsb);
        ASSERT_TRUE(f.open(QIODevice::Append));
        f.write(QByteArray(2048, 'z'));
    }
    const QString hash1 = Database::computeContentHash(docUsb);
    ASSERT_NE(hash0, hash1);

    // 注释后的文档保存书签：按新指纹全量替换，旧指纹书签不受影响
    ASSERT_TRUE(m_tester->saveBookmarks(docUsb, QSet<int> {0, 2, 4}));

    // 关键断言 1：注释后的文件（新指纹）书签正常
    read.clear();
    EXPECT_TRUE(m_tester->readBookmarks(docUsb, read));
    EXPECT_EQ(read, (QSet<int> {0, 2, 4}));

    // 关键断言 2：原始文件（旧指纹）书签保留可找回
    read.clear();
    EXPECT_TRUE(m_tester->readBookmarks(docLocal, read));
    EXPECT_EQ(read, (QSet<int> {0, 2, 4}));

    // 新旧指纹的书签记录共存（各一套，互不干扰）
    QSqlQuery cnt(m_tester->m_database);
    cnt.prepare("SELECT COUNT(DISTINCT contentHash) FROM bookmark WHERE contentHash IN (:h0, :h1)");
    cnt.bindValue(":h0", hash0);
    cnt.bindValue(":h1", hash1);
    ASSERT_TRUE(cnt.exec());
    ASSERT_TRUE(cnt.next());
    EXPECT_EQ(cnt.value(0).toInt(), 2);

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);
    QFile::remove(docLocal);
    QFile::remove(docUsb);
}


TEST_F(TestDatabase, UT_Database_bookmarkPathIndependence_001)
{
    // bug 场景：本地与U盘各有一份内容完全相同的文档（hash 相同、路径不同），
    // 书签曾以内容指纹为唯一关联键跨路径共享：在U盘文档上标记书签并关闭后，
    // 书签被同步到本地同内容文档上。修复后书签以 路径+指纹 为关联键：
    // 各路径书签相互独立，互不同步、互不删除。
    const QString dir = "/tmp/deepin_reader_ut_bookmark";
    QDir().mkpath(dir);
    QString src = UTSOURCEDIR;
    src += "/files/normal.pdf";
    const QString docLocal = dir + "/indep_local.pdf";
    const QString docUsb = dir + "/indep_usb.pdf";
    QFile::remove(docLocal);
    QFile::remove(docUsb);
    ASSERT_TRUE(QFile::copy(src, docLocal));
    ASSERT_TRUE(QFile::copy(src, docUsb));
    ASSERT_EQ(Database::computeContentHash(docLocal), Database::computeContentHash(docUsb));

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);

    // U盘文档标记书签 {1, 3} 后关闭保存
    ASSERT_TRUE(m_tester->saveBookmarks(docUsb, QSet<int> {1, 3}));

    // 本地同内容文档打开：不应读到U盘路径的书签
    QSet<int> read;
    EXPECT_TRUE(m_tester->readBookmarks(docLocal, read));
    EXPECT_TRUE(read.isEmpty());

    // 本地文档标记自己的书签 {5} 后关闭保存：U盘书签不受影响
    ASSERT_TRUE(m_tester->saveBookmarks(docLocal, QSet<int> {5}));
    read.clear();
    EXPECT_TRUE(m_tester->readBookmarks(docUsb, read));
    EXPECT_EQ(read, (QSet<int> {1, 3}));

    // U盘文档清空书签并保存：本地书签不受影响（不被连带删除）
    ASSERT_TRUE(m_tester->saveBookmarks(docUsb, QSet<int> {}));
    read.clear();
    EXPECT_TRUE(m_tester->readBookmarks(docLocal, read));
    EXPECT_EQ(read, (QSet<int> {5}));
    EXPECT_EQ(ut_bookmark_count(m_tester, docLocal), 1);
    EXPECT_EQ(ut_bookmark_count(m_tester, docUsb), 0);

    ut_cleanup_path(m_tester, docLocal);
    ut_cleanup_path(m_tester, docUsb);
    QFile::remove(docLocal);
    QFile::remove(docUsb);
}

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

    // 打开新路径：内容匹配迁移，书签与阅读状态都跟随
    EXPECT_TRUE(m_tester->matchOperationByContent(QFileInfo(docMoved), &sheet));
    EXPECT_EQ(ut_operation_count(m_tester, doc1), 0);
    EXPECT_EQ(ut_bookmark_count(m_tester, doc1), 0);

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

    // 文档1 第 1 页添加书签，正常读取
    ASSERT_TRUE(m_tester->saveBookmarks(doc1, QSet<int> {0}));
    QSet<int> read;
    EXPECT_TRUE(m_tester->readBookmarks(doc1, read));
    EXPECT_EQ(read, QSet<int> {0});

    // 文档1 被删除，文档2 改名为文档1（同路径、不同内容）
    QFile::remove(doc1);
    ASSERT_TRUE(QFile::rename(doc2, doc1));

    // 旧文件的书签不应出现在新文件上，且过期记录已被清除
    read.clear();
    EXPECT_TRUE(m_tester->readBookmarks(doc1, read));
    EXPECT_TRUE(read.isEmpty());
    EXPECT_EQ(ut_bookmark_count(m_tester, doc1), 0);

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

    QFile::remove(doc1);
}

TEST_F(TestDatabase, UT_Database_bookmarkContentHash_003)
{
    // 文件不可读（无法计算指纹）时跳过校验，按旧行为返回书签且不删除记录
    QString doc1, doc2;
    ut_bookmark_prepare_files(doc1, doc2);
    QFile::remove(doc2);

    ASSERT_TRUE(m_tester->saveBookmarks(doc1, QSet<int> {2}));

    // 移除读权限，使 computeContentHash 失败（root 下仍可读，不走该分支，但用例仍应通过）
    QFile::setPermissions(doc1, QFile::Permissions());

    QSet<int> read;
    EXPECT_TRUE(m_tester->readBookmarks(doc1, read));
    EXPECT_EQ(read, QSet<int> {2});
    EXPECT_EQ(ut_bookmark_count(m_tester, doc1), 1);

    QFile::setPermissions(doc1, QFile::ReadOwner | QFile::WriteOwner);
    QFile::remove(doc1);
}

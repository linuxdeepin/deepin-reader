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

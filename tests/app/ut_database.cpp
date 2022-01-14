/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     gaoxiang <gaoxiang@uniontech.com>
*
* Maintainer: gaoxiang <gaoxiang@uniontech.com>
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

#include "Database.h"
#include "Global.h"
#include "stub.h"
#include "DocSheet.h"

#include <QTest>

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

    EXPECT_TRUE(m_tester->readOperation(sheet));

    delete sheet;
}

TEST_F(TestDatabase, UT_Database_saveOperation_001)
{
    QString strPath = UTSOURCEDIR;
    strPath += "/files/normal.pdf";
    DocSheet *sheet = new DocSheet(Dr::FileType::PDF, strPath, nullptr);

    EXPECT_TRUE(m_tester->saveOperation(sheet));

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

//TEST_F(TestDatabase, UT_Database_saveBookmarks_001)
//{
//    QString strPath = UTSOURCEDIR;
//    strPath += "/files/normal.pdf";
//    QSet<int> bookmarks = {0, 1};
//    EXPECT_TRUE(m_tester->saveBookmarks(strPath, bookmarks));
//}


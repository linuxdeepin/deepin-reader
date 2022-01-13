/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chendu <chendu@uniontech.com>
*
* Maintainer: chendu <chendu@uniontech.com>
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

#include "DebugTimeManager.h"
#include "Global.h"
#include "stub.h"
#include "DocSheet.h"

#include <QTest>
#include <QTimer>

#include <gtest/gtest.h>

/********测试DebugTimeManager***********/
class TestDebugTimeManager : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    DebugTimeManager *m_tester = nullptr;
};
void TestDebugTimeManager::SetUp()
{
    m_tester = DebugTimeManager::getInstance();
}

void TestDebugTimeManager::TearDown()
{
    delete m_tester;
    DebugTimeManager::s_Instance = nullptr;

}

/*************测试用例****************/
TEST_F(TestDebugTimeManager, UT_DebugTimeManager_clear_001)
{
    m_tester->m_MapPoint.insert("first", {"first", 1000});
    m_tester->clear();
    EXPECT_TRUE(m_tester->m_MapPoint.isEmpty());
}

TEST_F(TestDebugTimeManager, UT_DebugTimeManager_beginPointQt_001)
{
    QString point("first");
    QString status("first");
    m_tester->beginPointQt(point, status);
    EXPECT_TRUE(m_tester->m_MapPoint.size() == 1);
}

TEST_F(TestDebugTimeManager, UT_DebugTimeManager_endPointQt_001)
{
//    QString point("first");
//    QString status("first");
//    m_tester->beginPointQt(point, status);

//    QEventLoop eventloop;
//    QTimer::singleShot(5, &eventloop, SLOT(quit()));
//    eventloop.exec();

//    m_tester->endPointQt(point);
//    EXPECT_TRUE(m_tester->m_MapPoint.isEmpty());
}

TEST_F(TestDebugTimeManager, UT_DebugTimeManager_beginPointLinux_001)
{
    QString point("first");
    QString status("first");
    m_tester->beginPointLinux(point, status);
    EXPECT_TRUE(m_tester->m_MapPoint.size() == 1);
}

TEST_F(TestDebugTimeManager, UT_DebugTimeManager_endPointLinux_001)
{
//    QString point("first");
//    QString status("first");
//    m_tester->beginPointQt(point, status);

//    QEventLoop eventloop;
//    QTimer::singleShot(5, &eventloop, SLOT(quit()));
//    eventloop.exec();

//    m_tester->endPointLinux(point);
//    EXPECT_TRUE(m_tester->m_MapPoint.isEmpty());
}

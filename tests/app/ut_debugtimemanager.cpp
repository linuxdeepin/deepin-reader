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

#include "DebugTimeManager.h"

#include <QTest>

#include <gtest/gtest.h>


/*******************************函数打桩************************************/

/*******************************函数打桩************************************/


class UT_DebugTimeManager : public ::testing::Test
{
public:
    UT_DebugTimeManager(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new DebugTimeManager;
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    DebugTimeManager *m_tester;
};

TEST_F(UT_DebugTimeManager, initTest)
{

}

TEST_F(UT_DebugTimeManager, UT_DebugTimeManager_clear)
{
    m_tester->m_MapPoint["1"] = PointInfo();
    m_tester->clear();
    EXPECT_EQ(m_tester->m_MapPoint.isEmpty(), true);
}

TEST_F(UT_DebugTimeManager, UT_DebugTimeManager_beginPointQt)
{
    m_tester->beginPointQt("1", "status");
    EXPECT_EQ(m_tester->m_MapPoint["1"].desc == "status", true);
}

TEST_F(UT_DebugTimeManager, UT_DebugTimeManager_endPointQt)
{
    m_tester->beginPointQt("1", "status");
    m_tester->endPointQt("1");
    EXPECT_EQ(m_tester->m_MapPoint.isEmpty(), true);
}

TEST_F(UT_DebugTimeManager, UT_DebugTimeManager_beginPointLinux)
{
    m_tester->beginPointLinux("1", "status");
    EXPECT_EQ(m_tester->m_MapPoint["1"].desc == "status", true);
}

TEST_F(UT_DebugTimeManager, UT_DebugTimeManager_endPointLinux)
{
    m_tester->beginPointLinux("1", "status");
    m_tester->endPointLinux("1");
    EXPECT_EQ(m_tester->m_MapPoint.isEmpty(), true);
}

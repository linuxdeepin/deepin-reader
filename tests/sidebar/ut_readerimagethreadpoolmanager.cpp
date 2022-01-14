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

#include "ReaderImageThreadPoolManager.h"

#include "ut_common.h"

#include "stub.h"

#include <gtest/gtest.h>

class UT_ReadImageTask : public ::testing::Test
{
public:
    UT_ReadImageTask(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new ReadImageTask();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ReadImageTask *m_tester;
};

TEST_F(UT_ReadImageTask, initTest)
{

}

TEST_F(UT_ReadImageTask, UT_ReadImageTask_addgetDocImageTask)
{
    ReaderImageParam_t readImageParam;
    readImageParam.pageIndex = 1;
    m_tester->addgetDocImageTask(readImageParam);
    EXPECT_TRUE(m_tester->m_docParam.pageIndex == 1);
}

TEST_F(UT_ReadImageTask, UT_ReadImageTask_setThreadPoolManager)
{
    QObject *object = new QObject();
    m_tester->setThreadPoolManager(object);
    EXPECT_TRUE(m_tester->m_threadpoolManager == object);
    delete object;
}

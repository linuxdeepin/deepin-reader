// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ReaderImageThreadPoolManager.h"

#include "ut_common.h"

#include "stub.h"
#include "addr_pri.h"

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

ACCESS_PRIVATE_FIELD(ReadImageTask, ReaderImageParam_t, m_docParam);
TEST_F(UT_ReadImageTask, UT_ReadImageTask_addgetDocImageTask)
{
    ReaderImageParam_t readImageParam;
    readImageParam.pageIndex = 1;
    m_tester->addgetDocImageTask(readImageParam);
    EXPECT_TRUE(access_private_field::ReadImageTaskm_docParam(*m_tester).pageIndex == 1);
}

ACCESS_PRIVATE_FIELD(ReadImageTask, QObject *, m_threadpoolManager);
TEST_F(UT_ReadImageTask, UT_ReadImageTask_setThreadPoolManager)
{
    QObject *object = new QObject();
    m_tester->setThreadPoolManager(object);
    EXPECT_TRUE(access_private_field::ReadImageTaskm_threadpoolManager(*m_tester) == object);
    delete object;
}

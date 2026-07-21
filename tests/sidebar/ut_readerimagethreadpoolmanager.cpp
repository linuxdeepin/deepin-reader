// Copyright (C) 2019-2026 ~ 2020 UnionTech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "ReaderImageThreadPoolManager.h"
#include "DocSheet.h"

#include "ut_common.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QPixmap>
#include <QImage>

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

TEST_F(UT_ReadImageTask, UT_ReadImageTask_runNoSheet)
{
    // run() with no sheet set should be safe (no crash, no-op)
    m_tester->run();
    SUCCEED();
}

class UT_ReaderImageThreadPoolManager : public ::testing::Test
{
public:
    void SetUp() override
    {
        m_tester = ReaderImageThreadPoolManager::getInstance();
    }

    void TearDown() override
    {
    }

protected:
    ReaderImageThreadPoolManager *m_tester;
};

TEST_F(UT_ReaderImageThreadPoolManager, initTest)
{
    EXPECT_NE(m_tester, nullptr);
}

TEST_F(UT_ReaderImageThreadPoolManager, UT_getInstanceSingleton)
{
    ReaderImageThreadPoolManager *instance1 = ReaderImageThreadPoolManager::getInstance();
    ReaderImageThreadPoolManager *instance2 = ReaderImageThreadPoolManager::getInstance();
    EXPECT_EQ(instance1, instance2);
}

TEST_F(UT_ReaderImageThreadPoolManager, UT_getImageForDocSheetEmpty)
{
    QPixmap pix = m_tester->getImageForDocSheet(nullptr, 0);
    EXPECT_TRUE(pix.isNull());
}

TEST_F(UT_ReaderImageThreadPoolManager, UT_onDocProxyDestroyedUnknown)
{
    QObject obj;
    m_tester->onDocProxyDestroyed(&obj);
    SUCCEED();
}

TEST_F(UT_ReaderImageThreadPoolManager, UT_onReceiverDestroyedUnknown)
{
    QObject obj;
    m_tester->onReceiverDestroyed(&obj);
    SUCCEED();
}

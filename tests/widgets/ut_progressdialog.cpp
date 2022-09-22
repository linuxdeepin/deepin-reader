// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ProgressDialog.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>

class TestProgressDialog : public ::testing::Test
{
public:
    TestProgressDialog(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new ProgressDialog();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    ProgressDialog *m_tester;
};

TEST_F(TestProgressDialog, initTest)
{

}

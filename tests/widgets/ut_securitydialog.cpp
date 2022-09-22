// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "SecurityDialog.h"

#include "stub.h"

#include <gtest/gtest.h>
#include <QTest>

class TestSecurityDialog : public ::testing::Test
{
public:
    TestSecurityDialog(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new SecurityDialog("123456789");
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    SecurityDialog *m_tester;
};

TEST_F(TestSecurityDialog, initTest)
{

}


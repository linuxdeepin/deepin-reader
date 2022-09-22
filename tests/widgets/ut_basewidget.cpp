// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "BaseWidget.h"
#include "stub.h"

#include <DGuiApplicationHelper>

#include <gtest/gtest.h>
#include <QTest>

class TestBaseWidget : public ::testing::Test
{
public:
    TestBaseWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new BaseWidget();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    BaseWidget *m_tester;
};

TEST_F(TestBaseWidget, initTest)
{

}

TEST_F(TestBaseWidget, testupdateWidgetTheme)
{
    m_tester->updateWidgetTheme();
    Dtk::Gui::DPalette plt = m_tester->palette();
    EXPECT_TRUE(plt.color(Dtk::Gui::DPalette::Background) == plt.color(Dtk::Gui::DPalette::Base));
}

TEST_F(TestBaseWidget, testadaptWindowSize)
{
    m_tester->adaptWindowSize(1.0);
}

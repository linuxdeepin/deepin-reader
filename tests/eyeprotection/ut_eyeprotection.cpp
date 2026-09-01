// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "EyeProtectionManager.h"
#include "EyeProtectionAction.h"
#include "RoundColorWidget.h"
#include "SheetBrowser.h"

#include <DWidget>
#include <QSignalSpy>

#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE

/********测试EyeProtectionManager/EyeProtectionAction***********/
class UT_EyeProtection : public ::testing::Test
{
public:
    virtual void SetUp();

    virtual void TearDown();

protected:
    DWidget *m_parent = nullptr;
};

void UT_EyeProtection::SetUp()
{
    m_parent = new DWidget;
}

void UT_EyeProtection::TearDown()
{
    EyeProtectionManager::instance()->setMode(EyeProtectionManager::Off);
    delete m_parent;
}

TEST_F(UT_EyeProtection, UT_EyeProtectionManager_colors_001)
{
    EXPECT_TRUE(EyeProtectionManager::instance()->pageBackgroundColor().isValid());
    EXPECT_TRUE(EyeProtectionManager::instance()->foregroundColor().isValid());
}

TEST_F(UT_EyeProtection, UT_EyeProtectionManager_setMode_001)
{
    QSignalSpy spy(EyeProtectionManager::instance(), &EyeProtectionManager::modeChanged);

    EyeProtectionManager::instance()->setMode(EyeProtectionManager::Classic);
    EXPECT_TRUE(EyeProtectionManager::instance()->mode() == EyeProtectionManager::Classic);
    EXPECT_EQ(spy.count(), 1);

    // 相同模式早退分支
    EyeProtectionManager::instance()->setMode(EyeProtectionManager::Classic);
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(UT_EyeProtection, UT_EyeProtectionAction_onModeChanged_001)
{
    EyeProtectionAction action(m_parent);

    EyeProtectionManager::instance()->setMode(EyeProtectionManager::Green);
    action.onModeChanged(3);

    RoundColorWidget *btn = action.defaultWidget()->findChild<RoundColorWidget *>("eye_2");
    ASSERT_NE(btn, nullptr);
    emit btn->clicked();    // initWidget 中 connect 的 lambda -> onBtnClicked
    EXPECT_TRUE(EyeProtectionManager::instance()->mode() == EyeProtectionManager::Green);
}

TEST_F(UT_EyeProtection, UT_SheetBrowser_eyeModeLambda_001)
{
    SheetBrowser browser;
    EyeProtectionManager::instance()->setMode(EyeProtectionManager::Night);
    EXPECT_TRUE(EyeProtectionManager::instance()->mode() == EyeProtectionManager::Night);
}

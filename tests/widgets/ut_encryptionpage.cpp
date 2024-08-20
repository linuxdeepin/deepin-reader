// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "EncryptionPage.h"
#include "ut_common.h"
#include "stub.h"
#include "addr_pri.h"
#include <DGuiApplicationHelper>

#include <QSignalSpy>
#include <QLineEdit>

#include <gtest/gtest.h>

class UT_EncryptionPage : public ::testing::Test
{
public:
    UT_EncryptionPage(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new EncryptionPage();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    EncryptionPage *m_tester;
};
ACCESS_PRIVATE_FIELD(EncryptionPage, DPasswordEdit *, m_password);
ACCESS_PRIVATE_FIELD(EncryptionPage, DPushButton *, m_nextbutton);

TEST_F(UT_EncryptionPage, initTest)
{

}

TEST_F(UT_EncryptionPage, UT_EncryptionPage_nextbuttonClicked)
{
    QSignalSpy spy(m_tester, SIGNAL(sigExtractPassword(const QString &)));
    m_tester->nextbuttonClicked();
    EXPECT_TRUE(spy.count() == 1);
}

TEST_F(UT_EncryptionPage, UT_EncryptionPage_wrongPassWordSlot)
{
    m_tester->wrongPassWordSlot();
    EXPECT_TRUE(m_tester->m_password->text().isEmpty() == true);
}

TEST_F(UT_EncryptionPage, UT_EncryptionPage_onPasswordChanged_001)
{
    access_private_field::EncryptionPagem_password(*m_tester)->setAlert(true);
    m_tester->onPasswordChanged();
    EXPECT_TRUE(access_private_field::EncryptionPagem_nextbutton(*m_tester)->isEnabled() == false);
}

TEST_F(UT_EncryptionPage, UT_EncryptionPage_onPasswordChanged_002)
{
    access_private_field::EncryptionPagem_password(*m_tester)->setAlert(true);
    access_private_field::EncryptionPagem_password(*m_tester)->setText("123");
    m_tester->onPasswordChanged();
    EXPECT_TRUE(access_private_field::EncryptionPagem_nextbutton(*m_tester)->isEnabled() == true);
}

TEST_F(UT_EncryptionPage, UT_EncryptionPage_onSetPasswdFocus)
{
    Stub stub;
    UTCommon::stub_QWidget_isVisible(stub, true);
    m_tester->onSetPasswdFocus();
    EXPECT_TRUE(access_private_field::EncryptionPagem_password(*m_tester) != nullptr);
}

ACCESS_PRIVATE_FUN(EncryptionPage, void(), onUpdateTheme);

TEST_F(UT_EncryptionPage, UT_EncryptionPage_onUpdateTheme)
{
    DPalette plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
    call_private_fun::EncryptionPageonUpdateTheme(*m_tester);
    EXPECT_TRUE(m_tester->palette().color(Dtk::Gui::DPalette::Background) == plt.color(Dtk::Gui::DPalette::Base));
}



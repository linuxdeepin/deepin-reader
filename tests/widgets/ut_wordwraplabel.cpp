// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "WordWrapLabel.h"
#include "stub.h"
#include "addr_pri.h"
#include <QPaintEvent>

#include <gtest/gtest.h>

class UT_WordWrapLabel : public ::testing::Test
{
public:
    UT_WordWrapLabel(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new WordWrapLabel();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    WordWrapLabel *m_tester;
};

TEST_F(UT_WordWrapLabel, initTest)
{

}
ACCESS_PRIVATE_FIELD(WordWrapLabel, QString, m_text);

TEST_F(UT_WordWrapLabel, UT_WordWrapLabel_setText)
{
    m_tester->setText("123");
    EXPECT_TRUE(access_private_field::WordWrapLabelm_text(*m_tester) == "123");
}

ACCESS_PRIVATE_FIELD(WordWrapLabel, int, m_margin);

TEST_F(UT_WordWrapLabel, UT_WordWrapLabel_setMargin)
{
    m_tester->setMargin(1);
    EXPECT_TRUE(access_private_field::WordWrapLabelm_margin(*m_tester) == 1);
}

ACCESS_PRIVATE_FUN(WordWrapLabel, void(QPaintEvent *event), paintEvent);
TEST_F(UT_WordWrapLabel, UT_WordWrapLabelt_paintEvent)
{
    QPaintEvent paint(QRect(m_tester->rect()));
    call_private_fun::WordWrapLabelpaintEvent(*m_tester, &paint);
    EXPECT_FALSE(m_tester->grab().isNull());
}

ACCESS_PRIVATE_FUN(WordWrapLabel, void(), adjustContent);
TEST_F(UT_WordWrapLabel, UT_WordWrapLabel_adjustContent)
{
    QFontMetrics fontMetris(m_tester->font());
    int wordHeight = fontMetris.boundingRect(0, 0, m_tester->width() - 2 * access_private_field::WordWrapLabelm_margin(*m_tester), 0, static_cast<int>(m_tester->alignment() | Qt::TextWrapAnywhere), access_private_field::WordWrapLabelm_text(*m_tester)).height();
    call_private_fun::WordWrapLabeladjustContent(*m_tester);
    EXPECT_TRUE(m_tester->height() == wordHeight);
}

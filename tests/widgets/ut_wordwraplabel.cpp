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

#include "WordWrapLabel.h"

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

TEST_F(UT_WordWrapLabel, UT_WordWrapLabel_setText)
{
    m_tester->setText("123");
    EXPECT_TRUE(m_tester->m_text == "123");
}

TEST_F(UT_WordWrapLabel, UT_WordWrapLabel_setMargin)
{
    m_tester->setMargin(1);
    EXPECT_TRUE(m_tester->m_margin == 1);
}

TEST_F(UT_WordWrapLabel, UT_WordWrapLabelt_paintEvent)
{
    QPaintEvent paint(QRect(m_tester->rect()));
    m_tester->paintEvent(&paint);
    EXPECT_FALSE(m_tester->grab().isNull());
}

TEST_F(UT_WordWrapLabel, UT_WordWrapLabel_adjustContent)
{
    QFontMetrics fontMetris(m_tester->font());
    int wordHeight = fontMetris.boundingRect(0, 0, m_tester->width() - 2 * m_tester->m_margin, 0, static_cast<int>(m_tester->alignment() | Qt::TextWrapAnywhere), m_tester->m_text).height();
    m_tester->adjustContent();
    EXPECT_TRUE(m_tester->height() == wordHeight);
}

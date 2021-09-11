/*
* Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
*
* Author:      zhangkai <zhangkai@uniontech.com>
* Maintainer:  zhangkai <zhangkai@uniontech.com>
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "TipsWidget.h"

#include <DPlatformWindowHandle>
#include <DGuiApplicationHelper>
#include <DFontSizeManager>

#include <QCoreApplication>
#include <QPaintEvent>
#include <QPainter>

#include <gtest/gtest.h>

class UT_TipsWidget : public ::testing::Test
{
public:
    void SetUp()
    {
        tWidget = new TipsWidget;
    }
    void TearDown()
    {
        delete tWidget;
    }
    TipsWidget *tWidget;
};

TEST_F(UT_TipsWidget, UT_TipsWidget_onUpdateTheme)
{
    tWidget->onUpdateTheme();
    DPalette plt = tWidget->palette();
    EXPECT_TRUE(plt.color(Dtk::Gui::DPalette::Background) == plt.color(Dtk::Gui::DPalette::Base));
}

TEST_F(UT_TipsWidget, UT_TipsWidget_setText)
{
    tWidget->setText("text");
    EXPECT_STREQ("text", tWidget->m_text.toStdString().c_str());
}

TEST_F(UT_TipsWidget, UT_TipsWidget_setAlignment)
{
    tWidget->setAlignment(Qt::AlignTop);
    EXPECT_EQ(Qt::AlignTop, tWidget->m_alignment);
}

TEST_F(UT_TipsWidget, UT_TipsWidget_adjustContent)
{
    QFontMetricsF fontMetris(tWidget->font());
    int wordHeight = static_cast<int>(fontMetris.boundingRect(QRectF(0, 0, tWidget->width() - 2 * tWidget->m_lrMargin, 0),
                                                              static_cast<int>(tWidget->m_alignment | Qt::TextWrapAnywhere), "text").height() + 2 * tWidget->m_tbMargin);
    tWidget->adjustContent("text");
    EXPECT_EQ(tWidget->height(), wordHeight);
}

TEST_F(UT_TipsWidget, UT_TipsWidget_PaintEvent)
{
    QPaintEvent paint(QRect(tWidget->rect()));
    tWidget->paintEvent(&paint);
    EXPECT_FALSE(tWidget->grab().isNull());
}

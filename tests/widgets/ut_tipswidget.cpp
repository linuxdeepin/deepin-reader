// Copyright (C) 2019 ~ 2020 UnionTech Software Technology Co.,Ltd
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TipsWidget.h"
#include "stub.h"
#include "addr_pri.h"
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
        m_tester = new TipsWidget;
    }
    void TearDown()
    {
        delete m_tester;
    }

protected:
    TipsWidget *m_tester;
};

TEST_F(UT_TipsWidget, inittest)
{

}
ACCESS_PRIVATE_FUN(TipsWidget, void(), onUpdateTheme);

TEST_F(UT_TipsWidget, UT_TipsWidget_onUpdateTheme)
{
    call_private_fun::TipsWidgetonUpdateTheme(*m_tester);
    DPalette plt = m_tester->palette();
    EXPECT_TRUE(plt.color(Dtk::Gui::DPalette::Background) == plt.color(Dtk::Gui::DPalette::Base));
}

ACCESS_PRIVATE_FIELD(TipsWidget, QString, m_text);

TEST_F(UT_TipsWidget, UT_TipsWidget_setText)
{
    m_tester->setText("text");
    EXPECT_STREQ("text", access_private_field::TipsWidgetm_text(*m_tester).toStdString().c_str());
}

ACCESS_PRIVATE_FIELD(TipsWidget, Qt::Alignment, m_alignment);

TEST_F(UT_TipsWidget, UT_TipsWidget_setAlignment)
{
    m_tester->setAlignment(Qt::AlignTop);
    EXPECT_EQ(Qt::AlignTop, access_private_field::TipsWidgetm_alignment(*m_tester));
}

ACCESS_PRIVATE_FIELD(TipsWidget, int, m_lrMargin);
TEST_F(UT_TipsWidget, UT_TipsWidget_setLeftRightMargin)
{
    m_tester->setLeftRightMargin(0);
    EXPECT_TRUE(access_private_field::TipsWidgetm_lrMargin(*m_tester) == 0);
}

ACCESS_PRIVATE_FIELD(TipsWidget, int, m_tbMargin);
TEST_F(UT_TipsWidget, UT_TipsWidget_setTopBottomMargin)
{
    m_tester->setTopBottomMargin(0);
    EXPECT_TRUE(access_private_field::TipsWidgetm_tbMargin(*m_tester) == 0);
}

ACCESS_PRIVATE_FIELD(TipsWidget, int, m_maxLineCount);
TEST_F(UT_TipsWidget, UT_TipsWidget_setMaxLineCount)
{
    m_tester->setMaxLineCount(0);
    EXPECT_TRUE(access_private_field::TipsWidgetm_maxLineCount(*m_tester)  == 0);
}

ACCESS_PRIVATE_FUN(TipsWidget, void(const QString &text), adjustContent);

TEST_F(UT_TipsWidget, UT_TipsWidget_adjustContent)
{
    QFontMetricsF fontMetris(m_tester->font());
    int wordHeight = static_cast<int>(fontMetris.boundingRect(QRectF(0, 0, m_tester->width() - 2 * access_private_field::TipsWidgetm_lrMargin(*m_tester), 0),
                                                              static_cast<int>(access_private_field::TipsWidgetm_alignment(*m_tester) | Qt::TextWrapAnywhere), "text").height() + 2 * access_private_field::TipsWidgetm_tbMargin(*m_tester));

    call_private_fun::TipsWidgetadjustContent(*m_tester, "text");
    EXPECT_EQ(m_tester->height(), wordHeight);
}

ACCESS_PRIVATE_FUN(TipsWidget, void(QPaintEvent *event), paintEvent);
TEST_F(UT_TipsWidget, UT_TipsWidget_PaintEvent)
{
    QPaintEvent paint(QRect(m_tester->rect()));
    call_private_fun::TipsWidgetpaintEvent(*m_tester, &paint);
    EXPECT_FALSE(m_tester->grab().isNull());
}

static QString g_funcname;
void qTimer_start_stub2()
{
    g_funcname = __FUNCTION__;
    return;
}

ACCESS_PRIVATE_FIELD(TipsWidget, bool, m_autoChecked);
ACCESS_PRIVATE_FUN(TipsWidget, void(QShowEvent *event), showEvent);

TEST_F(UT_TipsWidget, UT_TipsWidget_showEvent)
{
    Stub stub;
    stub.set((void(QTimer::*)())ADDR(QTimer, start), qTimer_start_stub2);
    access_private_field::TipsWidgetm_autoChecked(*m_tester) = true;
    QShowEvent *event = new QShowEvent();
    call_private_fun::TipsWidgetshowEvent(*m_tester, event);

    delete event;
    EXPECT_TRUE(g_funcname == "qTimer_start_stub2");
    stub.reset((void(QTimer::*)())ADDR(QTimer, start));
}

ACCESS_PRIVATE_FUN(TipsWidget, void(QHideEvent *event), hideEvent);
ACCESS_PRIVATE_FIELD(TipsWidget, QTimer, m_timer);
TEST_F(UT_TipsWidget, UT_TipsWidget_hideEvent)
{
    QHideEvent *event = new QHideEvent();
    call_private_fun::TipsWidgethideEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(access_private_field::TipsWidgetm_timer(*m_tester).isActive() == false);
}

TEST_F(UT_TipsWidget, UT_TipsWidget_setAutoChecked)
{
    m_tester->setAutoChecked(true);
    EXPECT_TRUE(access_private_field::TipsWidgetm_autoChecked(*m_tester) == true);
}

ACCESS_PRIVATE_FUN(TipsWidget, void(), onTimeOut);
TEST_F(UT_TipsWidget, UT_TipsWidget_onTimeOut)
{
    call_private_fun::TipsWidgetonTimeOut(*m_tester);
    EXPECT_TRUE(m_tester->isVisible() == false);
}

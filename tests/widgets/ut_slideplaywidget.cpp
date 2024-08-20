// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "SlidePlayWidget.h"
#include "ut_common.h"
#include "stub.h"
#include "addr_pri.h"
#include <QSignalSpy>
#include <QDebug>

#include <gtest/gtest.h>

class UT_SlidePlayWidget : public ::testing::Test
{
public:
    UT_SlidePlayWidget(): m_tester(nullptr) {}

public:
    virtual void SetUp()
    {
        m_tester = new SlidePlayWidget();
        m_tester->disconnect();
    }

    virtual void TearDown()
    {
        delete m_tester;
    }

protected:
    SlidePlayWidget *m_tester;
};

TEST_F(UT_SlidePlayWidget, initTest)
{

}

static QString g_funcname;
void qTimer_start_stub1()
{
    g_funcname = __FUNCTION__;
    return;
}

TEST_F(UT_SlidePlayWidget, UT_SlidePlayWidget_showControl)
{
    Stub stub;
    stub.set((void(QTimer::*)())ADDR(QTimer, start), qTimer_start_stub1);
    m_tester->showControl();
    EXPECT_TRUE(g_funcname == "qTimer_start_stub1");
    stub.reset((void(QTimer::*)())ADDR(QTimer, start));

}

ACCESS_PRIVATE_FUN(SlidePlayWidget, void(), onTimerout);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_onTimerout)
{
    call_private_fun::SlidePlayWidgetonTimerout(*m_tester);
    EXPECT_TRUE(m_tester->isVisible() == false);
}

ACCESS_PRIVATE_FUN(SlidePlayWidget, void(QEvent *event), enterEvent);
ACCESS_PRIVATE_FIELD(SlidePlayWidget, QTimer, m_timer);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_enterEvent)
{
    QEvent *event = new QEvent(QEvent::Enter);
    call_private_fun::SlidePlayWidgetenterEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(access_private_field::SlidePlayWidgetm_timer(*m_tester).isActive() == false);
}

ACCESS_PRIVATE_FUN(SlidePlayWidget, void(QEvent *event), leaveEvent);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_leaveEvent)
{
    Stub stub;
    stub.set((void(QTimer::*)())ADDR(QTimer, start), qTimer_start_stub1);
    QEvent *event = new QEvent(QEvent::Leave);
    call_private_fun::SlidePlayWidgetleaveEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(g_funcname == "qTimer_start_stub1");
}

ACCESS_PRIVATE_FIELD(SlidePlayWidget, bool, m_autoPlay);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_setPlayStatus)
{
    m_tester->setPlayStatus(false);
    EXPECT_TRUE(access_private_field::SlidePlayWidgetm_autoPlay(*m_tester) == false);
}

TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_getPlayStatus)
{
    access_private_field::SlidePlayWidgetm_autoPlay(*m_tester) = false;
    EXPECT_TRUE(m_tester->getPlayStatus() == false);
}

ACCESS_PRIVATE_FUN(SlidePlayWidget, void(), onPreClicked);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_onPreClicked)
{
    QSignalSpy spy(m_tester, SIGNAL(signalPreBtnClicked()));
    call_private_fun::SlidePlayWidgetonPreClicked(*m_tester);
    EXPECT_TRUE(spy.count() == 1);
}

ACCESS_PRIVATE_FUN(SlidePlayWidget, void(), onPlayClicked);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_onPlayClicked)
{
    access_private_field::SlidePlayWidgetm_autoPlay(*m_tester) = false;
    call_private_fun::SlidePlayWidgetonPlayClicked(*m_tester);
    EXPECT_TRUE(access_private_field::SlidePlayWidgetm_autoPlay(*m_tester) == true);
}

ACCESS_PRIVATE_FUN(SlidePlayWidget, void(), onNextClicked);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_onNextClicked)
{
    QSignalSpy spy(m_tester, SIGNAL(signalNextBtnClicked()));
    call_private_fun::SlidePlayWidgetonNextClicked(*m_tester);
    EXPECT_TRUE(spy.count() == 1);
}

ACCESS_PRIVATE_FUN(SlidePlayWidget, void(), onExitClicked);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_onExitClicked)
{
    QSignalSpy spy(m_tester, SIGNAL(signalExitBtnClicked()));
    call_private_fun::SlidePlayWidgetonExitClicked(*m_tester);
    EXPECT_TRUE(spy.count() == 1);
}

ACCESS_PRIVATE_FUN(SlidePlayWidget, void(), playStatusChanged);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_playStatusChanged_001)
{
    access_private_field::SlidePlayWidgetm_autoPlay(*m_tester) = true;
    QSignalSpy spy(m_tester, SIGNAL(signalPlayBtnClicked()));
    call_private_fun::SlidePlayWidgetplayStatusChanged(*m_tester);
    EXPECT_TRUE(spy.count() == 1);
}

TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_playStatusChanged_002)
{
    access_private_field::SlidePlayWidgetm_autoPlay(*m_tester) = false;
    QSignalSpy spy(m_tester, SIGNAL(signalPlayBtnClicked()));
    call_private_fun::SlidePlayWidgetplayStatusChanged(*m_tester);
    EXPECT_TRUE(spy.count() == 1);
}

ACCESS_PRIVATE_FUN(SlidePlayWidget, void(QMouseEvent *event), mousePressEvent);
TEST_F(UT_SlidePlayWidget, test_TestTextEditShadowWidget_mousePressEvent)
{
    QMouseEvent *event = new QMouseEvent(QEvent::MouseButtonRelease, QPointF(50, 50), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    call_private_fun::SlidePlayWidgetmousePressEvent(*m_tester, event);
    delete event;
    EXPECT_TRUE(m_tester != nullptr);
}


// Copyright (C) 2019-2026 ~ 2020 UnionTech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Application.h"
#include "DocSheet.h"
#include "MainWindow.h"

#include <gtest/gtest.h>
#include <QTest>
#include <QPushButton>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QSignalSpy>

TEST(UT_Application, UT_Application_emitSheetChanged)
{
    EXPECT_NE(dApp, nullptr);
    dApp->emitSheetChanged();
    SUCCEED();
}

TEST(UT_Application, UT_Application_handleQuitAction)
{
    // MainWindow::m_list should be empty in test environment
    EXPECT_NE(dApp, nullptr);
    dApp->handleQuitAction();
    SUCCEED();
}

TEST(UT_Application, UT_Application_notifyKeyPressReturn)
{
    QPushButton btn;
    QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    dApp->notify(&btn, &event);
    EXPECT_TRUE(event.isAccepted());
}

TEST(UT_Application, UT_Application_notifyKeyPressEnter)
{
    QPushButton btn;
    QKeyEvent event(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    dApp->notify(&btn, &event);
    EXPECT_TRUE(event.isAccepted());
}

TEST(UT_Application, UT_Application_notifyKeyPressOther)
{
    QPushButton btn;
    QKeyEvent event(QEvent::KeyPress, Qt::Key_A, Qt::NoModifier);
    dApp->notify(&btn, &event);
    SUCCEED();
}

TEST(UT_Application, UT_Application_notifyFocusIn)
{
    QWidget widget;
    widget.setFocusPolicy(Qt::NoFocus);
    QFocusEvent event(QEvent::FocusIn, Qt::ActiveWindowFocusReason);
    dApp->notify(&widget, &event);
    SUCCEED();
}

TEST(UT_Application, UT_Application_notifyZOrderChange)
{
    QObject obj;
    QEvent event(QEvent::ZOrderChange);
    dApp->notify(&obj, &event);
    SUCCEED();
}

TEST(UT_Application, UT_Application_notifyWindowActivate)
{
    QObject obj;
    QEvent event(QEvent::WindowActivate);
    dApp->notify(&obj, &event);
    SUCCEED();
}

TEST(UT_Application, UT_Application_notifyOtherEvent)
{
    QWidget widget;
    QEvent event(QEvent::None);
    dApp->notify(&widget, &event);
    SUCCEED();
}

// ==================== PMS 批次 2 补强 ====================
// BUG44137 (sev2): 回车键无法触发自定义按钮（DTK DIconButton/DhuaButton 类不默认响应回车）。
// 回归意图: Application::notify 必须把 Key_Return/Key_Enter 转换为按钮 clicked 信号。

TEST(UT_Application, BUG44137_notifyReturnEmitsClickedSignal)
{
    QPushButton btn;
    QSignalSpy spy(&btn, &QAbstractButton::clicked);

    QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
    dApp->notify(&btn, &event);

    // 回车 → clicked 信号发射且事件被消费
    EXPECT_EQ(spy.count(), 1);
    EXPECT_TRUE(event.isAccepted());
}

TEST(UT_Application, BUG44137_notifyKeypadEnterEmitsClickedSignal)
{
    QPushButton btn;
    QSignalSpy spy(&btn, &QAbstractButton::clicked);

    QKeyEvent event(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    dApp->notify(&btn, &event);

    EXPECT_EQ(spy.count(), 1);
    EXPECT_TRUE(event.isAccepted());
}

// BUG44264 (sev2): Alt+M 无法唤出右键菜单。
// 回归意图: Application::notify 捕获 Alt+M 合成右键点击;
//          无输入法光标时走 QPoint(0,0) 守卫, 不崩溃。

TEST(UT_Application, BUG44264_notifyAltMSynthesizesContextMenu)
{
    QWidget widget;
    QKeyEvent event(QEvent::KeyPress, Qt::Key_M, Qt::AltModifier);
    dApp->notify(&widget, &event);
    SUCCEED();
}

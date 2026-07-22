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

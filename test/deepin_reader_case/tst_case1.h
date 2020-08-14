#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <DApplicationSettings>
#include <DLog>

#include "MainWindow.h"
#include "DocSheet.h"

using namespace testing;

DWIDGET_USE_NAMESPACE

TEST(mainwindow, test1)
{
    EXPECT_EQ(1, 1);

    ASSERT_THAT(0, Eq(0));

    MainWindow *window = MainWindow::createWindow();

    window->show();

    EXPECT_EQ(nullptr, window);

}

TEST(sideber, test1)
{
    EXPECT_EQ(1, 1);

    ASSERT_THAT(0, Eq(0));

    Dtk::Core::DLogManager::registerConsoleAppender();

    Dtk::Core::DLogManager::registerFileAppender();

    MainWindow *window = MainWindow::createWindow();

    window->show();

}

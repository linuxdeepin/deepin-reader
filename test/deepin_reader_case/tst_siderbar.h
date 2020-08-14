#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>

#include "MainWindow.h"
#include "DocSheet.h"

using namespace testing;

DWIDGET_USE_NAMESPACE

TEST(sidebar, test1)
{
    EXPECT_EQ(1, 1);

    ASSERT_THAT(0, Eq(0));

    MainWindow *window = MainWindow::createWindow();

    window->show();

}

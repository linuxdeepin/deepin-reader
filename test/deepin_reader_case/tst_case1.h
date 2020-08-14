#pragma once

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <DApplicationSettings>
#include <DLog>

#include "MainWindow.h"
#include "DocSheet.h"

using namespace testing;

DWIDGET_USE_NAMESPACE

TEST(case1, set1)
{
    EXPECT_EQ(1, 1);

    ASSERT_THAT(0, Eq(0));

    DApplicationSettings savetheme;
    Q_UNUSED(savetheme)

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    MainWindow *window = MainWindow::createWindow();

    window->show();

//    DocSheet sheet(Dr::PDF, "/home/samson/Desktop/4.pdf");

//    window->addSheet(&sheet);

    EXPECT_EQ(nullptr, window);

//    ASSERT_THAT(0, DocSheet::firstThumbnail("/home/samson/Desktop/3.pdf"));

}

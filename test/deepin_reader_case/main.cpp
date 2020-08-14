#include "tst_mainwindow.h"
#include "tst_siderbar.h"
#include "Application.h"

#include <gtest/gtest.h>
#include <DApplicationSettings>
#include <DLog>
#include <QDebug>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    DApplicationSettings savetheme;
    Q_UNUSED(savetheme)
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    qDebug() << result;
    return a.exec();
}

#include "tst_case1.h"
#include "Application.h"

#include <gtest/gtest.h>
#include <QDebug>

int main(int argc, char *argv[])
{
    Application a(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();
    qDebug() << result;
    return a.exec();
}

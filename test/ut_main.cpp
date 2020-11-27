#include "Application.h"
#include "ut_defines.h"

#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "offscreen");

    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

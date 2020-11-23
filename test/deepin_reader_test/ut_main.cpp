#include "Application.h"
#include "ut_defines.h"

#include <gtest/gtest.h>

#include <DApplicationSettings>
#include <DLog>
#include <QDebug>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);

    return RUN_ALL_TESTS();
}

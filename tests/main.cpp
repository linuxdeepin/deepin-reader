#include "Application.h"
#include "ut_defines.h"
#include "Application.h"

#include <gtest/gtest.h>

DWIDGET_USE_NAMESPACE

#if defined(CMAKE_SAFETYTEST_ARG_ON)
#include <sanitizer/asan_interface.h>
#endif

int main(int argc, char *argv[])
{
    qputenv("QT_QPA_PLATFORM", "offscreen");

    ::testing::InitGoogleTest(&argc, argv);

    int c = RUN_ALL_TESTS();

#if defined(CMAKE_SAFETYTEST_ARG_ON)
    __sanitizer_set_report_path("asan.log");
#endif

    return c;
}

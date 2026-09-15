# SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

find_package(GTest REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test Gui)

# ut_printsettings: test toCupsOptions three-item mapping
add_executable(ut_printsettings ut_printsettings.cpp)
target_link_libraries(ut_printsettings PRIVATE
    batchprint-core
    GTest::GTest
    GTest::Main
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Gui
)
add_test(NAME ut_printsettings COMMAND ut_printsettings)

# ut_cupsclient: test via ICupsApi mock injection
add_executable(ut_cupsclient ut_cupsclient.cpp)
target_link_libraries(ut_cupsclient PRIVATE
    batchprint-core
    GTest::GTest
    GTest::Main
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Gui
)
add_test(NAME ut_cupsclient COMMAND ut_cupsclient)

# ut_formatconverter: test format detection and conversion paths
add_executable(ut_formatconverter ut_formatconverter.cpp)
target_link_libraries(ut_formatconverter PRIVATE
    batchprint-core
    batchprint-convert
    GTest::GTest
    GTest::Main
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Gui
)
add_test(NAME ut_formatconverter COMMAND ut_formatconverter)

# ut_notifyclient: test notification message formatting
add_executable(ut_notifyclient ut_notifyclient.cpp)
target_link_libraries(ut_notifyclient PRIVATE
    batchprint-core
    GTest::GTest
    GTest::Main
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Gui
)
add_test(NAME ut_notifyclient COMMAND ut_notifyclient)

enable_testing()

cmake_minimum_required(VERSION 3.16)
project(reader-sidebar-appearance-tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(DTK REQUIRED IMPORTED_TARGET dtk6widget)

set(READER_DIR "${CMAKE_CURRENT_LIST_DIR}/../../reader")
add_executable(test-sidebar-appearance
    main.cc
    ${READER_DIR}/sidebar/ThumbnailDelegate.cpp
    ${READER_DIR}/eyeprotection/EyeProtectionManager.cpp
    ${READER_DIR}/eyeprotection/EyeProtectionManager.h
    ${READER_DIR}/browser/NightFilter.cpp)
target_include_directories(test-sidebar-appearance PRIVATE
    ${READER_DIR} ${READER_DIR}/app ${READER_DIR}/sidebar
    ${READER_DIR}/eyeprotection ${READER_DIR}/browser)
target_link_libraries(test-sidebar-appearance PRIVATE Qt6::Widgets PkgConfig::DTK)

enable_testing()
add_test(NAME sidebar-appearance COMMAND test-sidebar-appearance)
set_tests_properties(sidebar-appearance PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")

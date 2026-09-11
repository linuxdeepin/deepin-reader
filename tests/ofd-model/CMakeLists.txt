cmake_minimum_required(VERSION 3.16)
project(ofd-model-check LANGUAGES CXX)

# Configure this directory directly to check OFD without linking all reader tests.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(PkgConfig REQUIRED)
pkg_check_modules(CAIRO REQUIRED IMPORTED_TARGET cairo)
if (QT_VERSION_MAJOR EQUAL 6)
    pkg_check_modules(DTKCORE REQUIRED IMPORTED_TARGET dtk6core)
else()
    pkg_check_modules(DTKCORE REQUIRED IMPORTED_TARGET dtkcore)
endif()
find_package(GTest REQUIRED)
find_path(ROFD_INCLUDE_DIR rofd.h)
find_library(ROFD_FFI_LIBRARY NAMES rofd_ffi)
if (NOT ROFD_INCLUDE_DIR OR NOT ROFD_FFI_LIBRARY)
    message(FATAL_ERROR "Set ROFD_INCLUDE_DIR and ROFD_FFI_LIBRARY to the matching rofd build")
endif()

get_filename_component(READER_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../.." ABSOLUTE)
add_executable(ofd-model-check main.cc
    ../document/ut_ofdmodel.cpp
    ../document/ut_navigation.cpp
    ../document/ut_catalogoutlinemodel.cpp
    ${READER_ROOT}/reader/sidebar/CatalogOutlineModel.cpp
    ${READER_ROOT}/reader/document/Navigation.cpp
    ${READER_ROOT}/reader/document/Navigation.h
    ${READER_ROOT}/reader/document/OfdModel.cpp
    ${READER_ROOT}/reader/document/OfdModel.h
    ${READER_ROOT}/reader/document/Model.h)
target_compile_definitions(ofd-model-check PRIVATE
    OFD_SUPPORT_ENABLED UTSOURCEDIR="${READER_ROOT}/tests")
target_include_directories(ofd-model-check PRIVATE
    ${ROFD_INCLUDE_DIR}
    ${READER_ROOT}/reader ${READER_ROOT}/reader/document ${READER_ROOT}/reader/app ${READER_ROOT}/reader/sidebar
    ${READER_ROOT}/tests ${READER_ROOT}/3rdparty/deepin-pdfium/include)
target_link_libraries(ofd-model-check PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets PkgConfig::CAIRO PkgConfig::DTKCORE
    GTest::gtest ${ROFD_FFI_LIBRARY})

enable_testing()
add_test(NAME ofd-model COMMAND ofd-model-check)
set_tests_properties(ofd-model PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_LOGGING_RULES=org.deepin.reader=false")

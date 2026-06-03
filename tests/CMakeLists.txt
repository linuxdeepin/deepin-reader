cmake_minimum_required(VERSION 3.13)

project(test-deepin-reader LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 查找依赖
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBJPEG REQUIRED libjpeg)
pkg_check_modules(DDJVU REQUIRED ddjvuapi)
pkg_check_modules(FREETYPE REQUIRED freetype2)

# Use pdfium in system default
if (NOT USE_PDFIUM_BUNDLE)
    pkg_check_modules(Deepin-pdfium REQUIRED IMPORTED_TARGET deepin-pdfium)
endif()

# 查找 Qt 版本（优先 Qt6）
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
set(QT_DESIRED_VERSION ${QT_VERSION_MAJOR})

message(STATUS ">>> Test building with Qt${QT_DESIRED_VERSION}")

# 定义必需的 Qt 组件
set(qt_required_components Core Gui Widgets Network Sql Svg Concurrent Xml Test)

if (QT_DESIRED_VERSION MATCHES 6)
    list(APPEND qt_required_components Core5Compat)
endif()

find_package(Qt${QT_DESIRED_VERSION} REQUIRED COMPONENTS ${qt_required_components})

if(NOT QT_DESIRED_VERSION MATCHES 6)
    find_package(Qt5DBus REQUIRED)
    find_package(Qt5PrintSupport QUIET)
endif()

# 设置 DTK 版本
if (QT_DESIRED_VERSION MATCHES 6)
    set(DTK_VERSION_MAJOR 6)
    find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED COMPONENTS Widget Gui Core)
    set(DTK_USE_TARGETS ON)
else()
    set(DTK_VERSION_MAJOR "")
    find_package(DtkWidget REQUIRED)
    find_package(DtkGui REQUIRED)
    find_package(DtkCore REQUIRED)
    set(DTK_USE_TARGETS OFF)
endif()

# 查找 Google Test
find_package(GTest REQUIRED)

# XPS 支持
option(XPS_SUPPORT "Enable XPS document format support" ON)
set(XPS_SUPPORT_RESOLVED ${XPS_SUPPORT})
set(XPS_DEPS_FOUND OFF)

if (XPS_SUPPORT)
    pkg_check_modules(XPS_DEPS QUIET libgxps cairo glib-2.0 gobject-2.0)
    if (XPS_DEPS_FOUND)
        message(STATUS ">>> XPS support enabled (libgxps dependencies detected)")
        add_compile_definitions(XPS_SUPPORT_ENABLED)
        set(XPS_SUPPORT_RESOLVED ON)
    else()
        message(WARNING ">>> XPS support disabled: missing libgxps/cairo/glib-2.0/gobject-2.0 dependencies")
        set(XPS_SUPPORT_RESOLVED OFF)
    endif()
else()
    set(XPS_SUPPORT_RESOLVED OFF)
endif()

set(XPS_SUPPORT_ENABLED ${XPS_SUPPORT_RESOLVED})

# ====== 收集 reader 源文件 ======
# 根目录源文件（排除 main.cpp）
file(GLOB READER_ROOT_SOURCES
    "${CMAKE_SOURCE_DIR}/reader/*.cpp"
    "${CMAKE_SOURCE_DIR}/reader/*.c"
    "${CMAKE_SOURCE_DIR}/reader/*.h"
)
list(REMOVE_ITEM READER_ROOT_SOURCES "${CMAKE_SOURCE_DIR}/reader/main.cpp")

# 各子模块源文件
file(GLOB_RECURSE APP_SOURCES     "${CMAKE_SOURCE_DIR}/reader/app/*.cpp" "${CMAKE_SOURCE_DIR}/reader/app/*.h")
file(GLOB_RECURSE BROWSER_SOURCES "${CMAKE_SOURCE_DIR}/reader/browser/*.cpp" "${CMAKE_SOURCE_DIR}/reader/browser/*.h")
file(GLOB_RECURSE DOCUMENT_SOURCES "${CMAKE_SOURCE_DIR}/reader/document/*.cpp" "${CMAKE_SOURCE_DIR}/reader/document/*.h")
file(GLOB_RECURSE SIDEBAR_SOURCES "${CMAKE_SOURCE_DIR}/reader/sidebar/*.cpp" "${CMAKE_SOURCE_DIR}/reader/sidebar/*.h")
file(GLOB_RECURSE UIFRAME_SOURCES "${CMAKE_SOURCE_DIR}/reader/uiframe/*.cpp" "${CMAKE_SOURCE_DIR}/reader/uiframe/*.h")
file(GLOB_RECURSE WIDGET_SOURCES  "${CMAKE_SOURCE_DIR}/reader/widgets/*.cpp" "${CMAKE_SOURCE_DIR}/reader/widgets/*.h")

# ====== 收集测试源文件 ======
file(GLOB TEST_ROOT_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
file(GLOB TEST_APP_SOURCES     "${CMAKE_CURRENT_SOURCE_DIR}/app/*.cpp")
file(GLOB TEST_BROWSER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/browser/*.cpp")
file(GLOB TEST_DOCUMENT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/document/*.cpp")
file(GLOB TEST_SIDEBAR_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/sidebar/*.cpp")
file(GLOB TEST_UIFRAME_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/uiframe/*.cpp")
file(GLOB TEST_WIDGET_SOURCES  "${CMAKE_CURRENT_SOURCE_DIR}/widgets/*.cpp")

# ====== 合并所有源文件 ======
set(ALL_SOURCES
    ${READER_ROOT_SOURCES}
    ${APP_SOURCES}
    ${BROWSER_SOURCES}
    ${DOCUMENT_SOURCES}
    ${SIDEBAR_SOURCES}
    ${UIFRAME_SOURCES}
    ${WIDGET_SOURCES}
    ${TEST_ROOT_SOURCES}
    ${TEST_APP_SOURCES}
    ${TEST_BROWSER_SOURCES}
    ${TEST_DOCUMENT_SOURCES}
    ${TEST_SIDEBAR_SOURCES}
    ${TEST_UIFRAME_SOURCES}
    ${TEST_WIDGET_SOURCES}
)

# ====== 定义测试可执行文件 ======
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# ====== 包含目录 ======
target_include_directories(${PROJECT_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/reader
    ${CMAKE_SOURCE_DIR}/reader/app
    ${CMAKE_SOURCE_DIR}/reader/browser
    ${CMAKE_SOURCE_DIR}/reader/document
    ${CMAKE_SOURCE_DIR}/reader/sidebar
    ${CMAKE_SOURCE_DIR}/reader/uiframe
    ${CMAKE_SOURCE_DIR}/reader/widgets
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/gtest
    ${PDFIUM_INCLUDE_DIRS}
    $<$<BOOL:${XPS_SUPPORT_ENABLED}>:${XPS_DEPS_INCLUDE_DIRS}>
)

# DTK 包含目录（Qt5 使用变量，Qt6 使用目标）
if(NOT QT_DESIRED_VERSION MATCHES 6)
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${DtkWidget_INCLUDE_DIRS}
        ${DtkGui_INCLUDE_DIRS}
        ${DtkCore_INCLUDE_DIRS}
    )
endif()

# ====== 编译定义 ======
target_compile_definitions(${PROJECT_NAME} PRIVATE
    QT_DEPRECATED_WARNINGS
    UTSOURCEDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    APP_VERSION="${APP_VERSION}"
    INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
)

if (QT_DESIRED_VERSION MATCHES 6)
    target_compile_definitions(${PROJECT_NAME} PRIVATE QT_VERSION_6)
endif()

# ====== 编译选项 ======
target_compile_options(${PROJECT_NAME} PRIVATE
    -g -Wall -fno-access-control -O0 -fno-inline
    -fprofile-arcs -ftest-coverage
    -fstack-protector-strong -D_FORTIFY_SOURCE=1 -fPIC
    $<$<BOOL:${XPS_SUPPORT_ENABLED}>:${XPS_DEPS_CFLAGS_OTHER}>
)

target_link_options(${PROJECT_NAME} PRIVATE
    -g -Wall -fprofile-arcs -ftest-coverage -O0
    -pie -z lazy
)

# ====== 链接库 ======
set(TEST_LINK_LIBS
    Qt${QT_DESIRED_VERSION}::Core
    Qt${QT_DESIRED_VERSION}::Gui
    Qt${QT_DESIRED_VERSION}::Widgets
    Qt${QT_DESIRED_VERSION}::Network
    Qt${QT_DESIRED_VERSION}::Sql
    Qt${QT_DESIRED_VERSION}::Svg
    Qt${QT_DESIRED_VERSION}::Concurrent
    Qt${QT_DESIRED_VERSION}::Xml
    Qt${QT_DESIRED_VERSION}::Test
    GTest::GTest
    ${LIBJPEG_LIBRARIES}
    ${DDJVU_LIBRARIES}
    ${FREETYPE_LIBRARIES}
)

if (QT_DESIRED_VERSION MATCHES 6)
    list(APPEND TEST_LINK_LIBS Qt${QT_DESIRED_VERSION}::Core5Compat)
else()
    list(APPEND TEST_LINK_LIBS
        Qt5::DBus
        dl
        pthread
    )
    if(Qt5PrintSupport_FOUND)
        list(APPEND TEST_LINK_LIBS Qt5::PrintSupport)
    endif()
endif()

# DTK 链接
if(DTK_USE_TARGETS)
    list(APPEND TEST_LINK_LIBS
        Dtk${DTK_VERSION_MAJOR}::Widget
        Dtk${DTK_VERSION_MAJOR}::Gui
        Dtk${DTK_VERSION_MAJOR}::Core
    )
else()
    list(APPEND TEST_LINK_LIBS
        ${DtkWidget_LIBRARIES}
        ${DtkGui_LIBRARIES}
        ${DtkCore_LIBRARIES}
    )
endif()

# XPS 链接
if (XPS_SUPPORT_ENABLED)
    list(APPEND TEST_LINK_LIBS ${XPS_DEPS_LIBRARIES})
endif()

# PDFium 链接
if (USE_PDFIUM_BUNDLE)
    target_include_directories(${PROJECT_NAME} PUBLIC
        ${CMAKE_SOURCE_DIR}/3rdparty/deepin-pdfium/include
    )
    # 需要先构建 deepin-pdfium
    target_link_libraries(${PROJECT_NAME} PRIVATE ${TEST_LINK_LIBS} deepin-pdfium-reader)
else()
    list(APPEND TEST_LINK_LIBS PkgConfig::Deepin-pdfium)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${TEST_LINK_LIBS})
endif()

# ====== 资源文件 =====#
if (QT_DESIRED_VERSION MATCHES 6)
    qt6_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/files.qrc)
else()
    qt5_add_resources(RESOURCES ${CMAKE_CURRENT_SOURCE_DIR}/files.qrc)
endif()
target_sources(${PROJECT_NAME} PRIVATE ${RESOURCES})

# 启用测试
enable_testing()
add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

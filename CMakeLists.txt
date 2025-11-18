cmake_minimum_required(VERSION 3.15)

# 项目基本信息
project(deepin-reader
    VERSION 1.0.0
    DESCRIPTION "Deepin Document Viewer"
    LANGUAGES CXX C
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# XPS支持选项（根据依赖检测自动启用）
find_package(PkgConfig REQUIRED)
option(XPS_SUPPORT "Enable XPS document format support" ON)

set(XPS_SUPPORT_RESOLVED ${XPS_SUPPORT})
set(XPS_DEPS_FOUND OFF)

if (XPS_SUPPORT)
    if (PkgConfig_FOUND)
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
        message(WARNING ">>> XPS support disabled: pkg-config not available")
        set(XPS_SUPPORT_RESOLVED OFF)
    endif()
else()
    message(STATUS ">>> XPS support disabled by configuration")
endif()

set(XPS_SUPPORT_ENABLED ${XPS_SUPPORT_RESOLVED})
if (XPS_SUPPORT_ENABLED)
    set(XPS_DEPS_INCLUDE_DIRS ${XPS_DEPS_INCLUDE_DIRS})
    set(XPS_DEPS_LIBRARIES ${XPS_DEPS_LIBRARIES})
    set(XPS_DEPS_CFLAGS_OTHER ${XPS_DEPS_CFLAGS_OTHER})
else()
    unset(XPS_DEPS_INCLUDE_DIRS)
    unset(XPS_DEPS_LIBRARIES)
    unset(XPS_DEPS_CFLAGS_OTHER)
endif()

include(GNUInstallDirs)
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
   set(CMAKE_INSTALL_PREFIX /usr)
endif ()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
# 引入翻译生成
include(translation-generate)

# 查找Qt版本
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
message(">>> Found Qt version: ${QT_VERSION_MAJOR}")
set(QT_DESIRED_VERSION ${QT_VERSION_MAJOR})

set(PDF_LIB_VERSION "")

# 设置DTK版本
if (QT_VERSION_MAJOR MATCHES 6)
    find_package(Dtk6CMake REQUIRED)
    set(DTK_VERSION_MAJOR 6)
else()
    find_package(DtkCMake REQUIRED)
    set(DTK_VERSION_MAJOR "")
endif()

if(CMAKE_SYSTEM_PROCESSOR MATCHES "mips64")
    if (${DEEPIN_OS_VERSION} MATCHES "25")
        set(PDF_LIB_VERSION 5)
    endif()
endif()

message(">>> Build with DTK: ${DTK_VERSION_MAJOR}")

# 定义必需的Qt组件
set(qt_required_components Core Gui Widgets Network Sql Svg Concurrent Xml)

# 添加Qt6特有组件
if (QT_DESIRED_VERSION MATCHES 6)
    list(APPEND qt_required_components Core5Compat)
endif()

# 查找Qt和DTK
find_package(Qt${QT_DESIRED_VERSION} REQUIRED COMPONENTS ${qt_required_components})
find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED COMPONENTS Widget Gui Core)

# 定义链接库
set(LINK_LIBS
    Qt${QT_DESIRED_VERSION}::Core
    Qt${QT_DESIRED_VERSION}::Gui
    Qt${QT_DESIRED_VERSION}::Widgets
    Qt${QT_DESIRED_VERSION}::Network
    Qt${QT_DESIRED_VERSION}::Sql
    Qt${QT_DESIRED_VERSION}::Svg
    Qt${QT_DESIRED_VERSION}::Concurrent
    Qt${QT_DESIRED_VERSION}::Xml
    Dtk${DTK_VERSION_MAJOR}::Widget
    Dtk${DTK_VERSION_MAJOR}::Gui
    Dtk${DTK_VERSION_MAJOR}::Core
)

# 添加Qt6特有链接库
if (QT_DESIRED_VERSION MATCHES 6)
    list(APPEND LINK_LIBS Qt${QT_DESIRED_VERSION}::Core5Compat)
endif()

# 安全编译选项
add_compile_options(
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -fPIC
    -pie
    -Wno-unused-parameter
)

add_link_options(
    -pie
    -z lazy
)

# 添加子项目
add_subdirectory(reader)
add_subdirectory(htmltopdf)

# translation files
TRANSLATION_GENERATE(QM_FILES ${CMAKE_SOURCE_DIR}/translations)
add_custom_target(${PROJECT_NAME}_qm_files DEPENDS ${QM_FILES})
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_qm_files)

# Install translations
install(FILES ${QM_FILES} DESTINATION share/${PROJECT_NAME}/translations)

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

# XPS支持选项（默认关闭）
option(XPS_SUPPORT "Enable XPS document format support" OFF)
set (XPS_SUPPORT ON)
if(XPS_SUPPORT)
    message(">>> XPS support enabled")
    add_compile_definitions(XPS_SUPPORT_ENABLED)
else()
    message(">>> XPS support disabled")
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

# 设置DTK版本
if (QT_VERSION_MAJOR MATCHES 6)
    set(DTK_VERSION_MAJOR 6)
else()
    set(DTK_VERSION_MAJOR "")
endif()
set(PDF_LIB_VERSION "")
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

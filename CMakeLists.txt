cmake_minimum_required(VERSION 3.13)

# 项目基本信息
project(deepin-reader
    VERSION 1.0.0
    DESCRIPTION "Deepin Document Viewer"
    LANGUAGES CXX C
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    message("Enable build hardening.")

    set(CMAKE_VERBOSE_MAKEFILE ON)

    set(HARDENING_FLAGS "-Wdate-time -D_FORTIFY_SOURCE=2 -g -O2 -ffile-prefix-map=${CMAKE_SOURCE_DIR}=. -fstack-protector-strong -fstack-clash-protection -Wformat -Werror=format-security")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${HARDENING_FLAGS}")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${HARDENING_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro -Wl,-z,now")
endif()

# Option to use bundled PDFium
option(USE_PDFIUM_BUNDLE "Use bundled PDFium library instead of system version" ON)

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

# OFD支持选项（依赖发行版提供的 rofd C ABI 库，见 debian/control 的 librofd-ffi-dev）
option(OFD_SUPPORT "Enable OFD document format support" ON)
set(ROFD_ROOT "" CACHE PATH "Path to the rofd repository checkout or install prefix, overrides the system install")

set(OFD_SUPPORT_RESOLVED ${OFD_SUPPORT})

if (OFD_SUPPORT)
    # 默认走系统路径：librofd-ffi-dev 提供 /usr/include/rofd.h 与 lib/<multiarch>/librofd_ffi.so
    set(ROFD_ROOT_HINTS "")
    if (ROFD_ROOT)
        list(APPEND ROFD_ROOT_HINTS ${ROFD_ROOT})
    endif()
    if (EXISTS "${CMAKE_SOURCE_DIR}/../rofd")
        list(APPEND ROFD_ROOT_HINTS "${CMAKE_SOURCE_DIR}/../rofd")
    endif()

    set(ROFD_LIB_SUFFIXES target/release target/debug lib)
    if (CMAKE_LIBRARY_ARCHITECTURE)
        list(APPEND ROFD_LIB_SUFFIXES "lib/${CMAKE_LIBRARY_ARCHITECTURE}")
    endif()

    find_path(ROFD_INCLUDE_DIR rofd.h
        HINTS ${ROFD_ROOT_HINTS}
        PATH_SUFFIXES crates/rofd-ffi/include include
    )
    find_library(ROFD_FFI_LIBRARY NAMES rofd_ffi
        HINTS ${ROFD_ROOT_HINTS}
        PATH_SUFFIXES ${ROFD_LIB_SUFFIXES}
    )
    pkg_check_modules(OFD_CAIRO QUIET cairo)

    if (ROFD_INCLUDE_DIR AND ROFD_FFI_LIBRARY AND OFD_CAIRO_FOUND)
        # These APIs were added after 0.3.0 without a version bump. Probe both
        # declarations and linked symbols instead of trusting the package version.
        include(CheckCSourceCompiles)
        include(CMakePushCheckState)
        cmake_push_check_state(RESET)
        set(CMAKE_REQUIRED_INCLUDES ${ROFD_INCLUDE_DIR} ${OFD_CAIRO_INCLUDE_DIRS})
        set(CMAKE_REQUIRED_LIBRARIES ${ROFD_FFI_LIBRARY} ${OFD_CAIRO_LIBRARIES})
        unset(ROFD_READER_APIS_AVAILABLE CACHE)
        check_c_source_compiles([=[
            #include <rofd.h>
            int main(void) {
                rofd_pixel_rect_t viewport;
                int32_t w = 0, h = 0;
                rofd_pixel_rect_init(&viewport, sizeof(viewport));
                rofd_renderer_get_pixel_canvas_size(0, 0, 0, &w, &h, 0);
                rofd_renderer_render_page_region_cairo(0, 0, 0, 0, &viewport, 0, 0);
                return 0;
            }
        ]=] ROFD_READER_APIS_AVAILABLE)
        cmake_pop_check_state()
        if (NOT ROFD_READER_APIS_AVAILABLE)
            message(FATAL_ERROR "rofd headers/library lack the region rendering APIs. Use a matching rofd build containing commit 56da925 (0.3.0 alone is insufficient), and set ROFD_INCLUDE_DIR/ROFD_FFI_LIBRARY accordingly.")
        endif()
        message(STATUS ">>> OFD support enabled (rofd_ffi: ${ROFD_FFI_LIBRARY})")
        add_compile_definitions(OFD_SUPPORT_ENABLED)
        set(OFD_SUPPORT_RESOLVED ON)
    else()
        message(WARNING ">>> OFD support disabled: install librofd-ffi-dev, or pass -DROFD_ROOT=<rofd checkout>")
        set(OFD_SUPPORT_RESOLVED OFF)
    endif()
else()
    message(STATUS ">>> OFD support disabled by configuration")
endif()

set(OFD_SUPPORT_ENABLED ${OFD_SUPPORT_RESOLVED})

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

message(">>> Build with DTK: ${DTK_VERSION_MAJOR}")

# 定义必需的Qt组件
set(qt_required_components Core Gui Widgets Network Sql Svg Concurrent Xml)

# 添加Qt6特有组件
if (QT_DESIRED_VERSION MATCHES 6)
    list(APPEND qt_required_components Core5Compat)
endif()

# 查找Qt和DTK
find_package(Qt${QT_DESIRED_VERSION} REQUIRED COMPONENTS ${qt_required_components})

if(NOT QT_DESIRED_VERSION MATCHES 6)
    find_package(Qt5DBus REQUIRED)
    find_package(Qt5PrintSupport QUIET)
endif()

if(DTK_VERSION_MAJOR)
    find_package(Dtk${DTK_VERSION_MAJOR} REQUIRED COMPONENTS Widget Gui Core)
    set(DTK_USE_TARGETS ON)
else()
    find_package(DtkWidget REQUIRED)
    find_package(DtkGui REQUIRED)
    find_package(DtkCore REQUIRED)
    set(DTK_USE_TARGETS OFF)
endif()

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
)

if(NOT QT_DESIRED_VERSION MATCHES 6)
    list(APPEND LINK_LIBS Qt5::DBus)
    if(Qt5PrintSupport_FOUND)
        list(APPEND LINK_LIBS Qt5::PrintSupport)
    endif()
endif()

# Qt6使用命名空间目标，Qt5使用变量
if(DTK_USE_TARGETS)
    list(APPEND LINK_LIBS
        Dtk${DTK_VERSION_MAJOR}::Widget
        Dtk${DTK_VERSION_MAJOR}::Gui
        Dtk${DTK_VERSION_MAJOR}::Core
    )
endif()

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

if (USE_PDFIUM_BUNDLE)
    add_subdirectory(3rdparty/deepin-pdfium)
endif()

# 单元测试（可选）
option(BUILD_TESTS "Build unit tests" OFF)
if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

# translation files
TRANSLATION_GENERATE(QM_FILES ${CMAKE_SOURCE_DIR}/translations)
add_custom_target(${PROJECT_NAME}_qm_files DEPENDS ${QM_FILES})
add_dependencies(${PROJECT_NAME} ${PROJECT_NAME}_qm_files)

# Install translations
install(FILES ${QM_FILES} DESTINATION share/${PROJECT_NAME}/translations)

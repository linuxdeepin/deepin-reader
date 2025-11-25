cmake_minimum_required(VERSION 3.15)

set(VERSION "1.0.0" CACHE STRING "PDF rendering library based on PDFium")

project(deepin-pdfium-reader
    VERSION ${VERSION}
    DESCRIPTION "PDF rendering library based on PDFium"
    LANGUAGES CXX C
)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 编译选项
add_compile_options(
    -fstack-protector-strong 
    -D_FORTITY_SOURCE=1 
    -z noexecstack 
    -pie 
    -fPIC 
    -Wno-unused-parameter
)

# 添加链接选项
add_link_options(
    -z lazy
)

# 获取系统页大小
execute_process(
    COMMAND getconf PAGESIZE
    OUTPUT_VARIABLE SYSTEM_PAGE_SIZE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_definitions(-DSYSTEMPAGESIZE=${SYSTEM_PAGE_SIZE})

# 查找Qt6依赖包
find_package(Qt${QT_DESIRED_VERSION} REQUIRED COMPONENTS Core Gui)
find_package(PkgConfig REQUIRED)
pkg_check_modules(DEPS REQUIRED
    chardet
    lcms2  
    freetype2
    libopenjp2
)

# PDFium 第三方库
add_subdirectory(src/3rdparty/pdfium)

# 定义导出宏
# add_definitions(-DBUILD_DEEPDF_LIB)

# 主库目标
add_library(${PROJECT_NAME} SHARED
    include/dpdfglobal.h
    include/dpdfdoc.h
    include/dpdfpage.h
    include/dpdfannot.h
    src/dpdfglobal.cpp
    src/dpdfdoc.cpp  
    src/dpdfpage.cpp
    src/dpdfannot.cpp
)

# 设置库的版本信息和 SOVERSION
set_target_properties(${PROJECT_NAME} PROPERTIES
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
)

target_include_directories(${PROJECT_NAME} 
    PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/deepin-pdfium>
    PRIVATE
        ${DEPS_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        pdfium
        Qt${QT_DESIRED_VERSION}::Core
        Qt${QT_DESIRED_VERSION}::Gui
        ${DEPS_LIBRARIES}
        z
        jpeg
        icuuc
)

# 安装
include(GNUInstallDirs)

install(TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

if (EXPORT_DEV_FILES)
install(FILES 
    include/dpdfglobal.h
    include/dpdfdoc.h
    include/dpdfpage.h 
    include/dpdfannot.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/deepin-pdfium
)

# pkg-config
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/deepin-pdfium.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/deepin-pdfium.pc
    @ONLY
)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/deepin-pdfium.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

include(CMakePackageConfigHelpers)

set(INCLUDE_INSTALL_DIR ${CMAKE_INSTALL_INCLUDEDIR}/deepin-pdfium)
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR})

configure_package_config_file(
    misc/deepin-pdfiumConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/deepin-pdfiumConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/deepin-pdfium
    PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/deepin-pdfiumConfig.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/deepin-pdfium
)

install(TARGETS ${PROJECT_NAME}
    EXPORT deepin-pdfiumTargets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(EXPORT deepin-pdfiumTargets
    FILE deepin-pdfiumTargets.cmake
    NAMESPACE deepin-pdfium::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/deepin-pdfium
)
endif (EXPORT_DEV_FILES)

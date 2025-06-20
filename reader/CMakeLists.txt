# 查找pkg-config依赖
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBJPEG REQUIRED libjpeg)
pkg_check_modules(DDJVU REQUIRED ddjvuapi)
pkg_check_modules(PDFIUM REQUIRED deepin-pdfium${PDF_LIB_VERSION})

# 添加位置无关代码编译标志
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIE")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pie")

# 收集所有源文件
file(GLOB SOURCES 
    "*.cpp" 
    "*.c"
    "*.h"
)

# app sources
file(GLOB_RECURSE APP_SOURCES 
    "app/*.cpp" 
    "app/*.h"
)
# browser sources
file(GLOB_RECURSE BROWSER_SOURCES 
    "browser/*.cpp" 
    "browser/*.h"
)
# document sources
file(GLOB_RECURSE DOCUMENT_SOURCES 
    "document/*.cpp" 
    "document/*.h"
)
# sidebar sources
file(GLOB_RECURSE SIDEBAR_SOURCES 
    "sidebar/*.cpp" 
    "sidebar/*.h"
)
# uiframe sources
file(GLOB_RECURSE UIFRAME_SOURCES 
    "uiframe/*.cpp" 
    "uiframe/*.h"
)
# widgets sources
file(GLOB_RECURSE WIDGET_SOURCES 
    "widgets/*.cpp" 
    "widgets/*.h"
)

# 定义reader可执行文件
add_executable(deepin-reader
    ${SOURCES}
    ${APP_SOURCES}
    ${BROWSER_SOURCES}
    ${DOCUMENT_SOURCES}
    ${SIDEBAR_SOURCES}
    ${UIFRAME_SOURCES}
    ${WIDGET_SOURCES}
)

# 启用自动moc处理
set_target_properties(deepin-reader PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
)

# 包含目录
target_include_directories(deepin-reader PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/app
    ${CMAKE_CURRENT_SOURCE_DIR}/browser
    ${CMAKE_CURRENT_SOURCE_DIR}/document
    ${CMAKE_CURRENT_SOURCE_DIR}/sidebar
    ${CMAKE_CURRENT_SOURCE_DIR}/uiframe
    ${CMAKE_CURRENT_SOURCE_DIR}/widgets
    ${PDFIUM_INCLUDE_DIRS}
)

if (NOT APP_VERSION)
    set(APP_VERSION "1.0.0")
endif()

# 定义安装前缀
target_compile_definitions(${PROJECT_NAME} PRIVATE
    INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
    APP_VERSION="${APP_VERSION}"
)

# 链接库
target_link_libraries(deepin-reader PRIVATE
    ${LINK_LIBS}
    ${LIBJPEG_LIBRARIES}
    ${DDJVU_LIBRARIES}
    ${PDFIUM_LIBRARIES}
)

# 添加资源文件
qt_add_resources(reader_RESOURCES
    ${CMAKE_SOURCE_DIR}/resources/resources.qrc
)
target_sources(deepin-reader PRIVATE ${reader_RESOURCES})

# 安装目标
install(TARGETS deepin-reader
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# 安装桌面文件和图标
install(FILES
    deepin-reader.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
)

install(FILES
    deepin-reader.svg
    DESTINATION ${CMAKE_INSTALL_DATADIR}/icons/hicolor/scalable/apps
)

# 安装帮助文件
install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/assets/
    DESTINATION ${CMAKE_INSTALL_DATADIR}/deepin-manual/manual-assets/application
)
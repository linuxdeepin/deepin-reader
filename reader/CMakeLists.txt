# 查找pkg-config依赖
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBJPEG REQUIRED libjpeg)
pkg_check_modules(DDJVU REQUIRED ddjvuapi)

if (XPS_SUPPORT_ENABLED)
    pkg_check_modules(XPS_DEPS REQUIRED libgxps cairo glib-2.0 gobject-2.0 freetype2)
endif()

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
    $<$<BOOL:${XPS_SUPPORT_ENABLED}>:${XPS_DEPS_INCLUDE_DIRS}>
)

# 使用DTK变量包含目录
if(NOT QT_VERSION_MAJOR MATCHES 6)
    target_include_directories(deepin-reader PUBLIC
        ${DtkWidget_INCLUDE_DIRS}
        ${DtkGui_INCLUDE_DIRS}
        ${DtkCore_INCLUDE_DIRS}
    )
endif()

if (NOT APP_VERSION)
    set(APP_VERSION "1.0.0")
endif()

# 定义安装前缀
target_compile_definitions(${PROJECT_NAME} PRIVATE
    INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
    APP_VERSION="${APP_VERSION}"
)

if (XPS_SUPPORT_ENABLED)
    target_compile_options(deepin-reader PRIVATE ${XPS_DEPS_CFLAGS_OTHER})
endif()

# 添加资源文件
set(READER_LINK_LIBS
    ${LINK_LIBS}
    deepin-pdfium-reader
    ${LIBJPEG_LIBRARIES}
    ${DDJVU_LIBRARIES}
)

if (XPS_SUPPORT_ENABLED)
    list(APPEND READER_LINK_LIBS ${XPS_DEPS_LIBRARIES})
endif()

if (QT_VERSION_MAJOR MATCHES 6)
    target_link_libraries(deepin-reader PRIVATE ${READER_LINK_LIBS})

    qt_add_resources(reader_RESOURCES
        ${CMAKE_SOURCE_DIR}/resources/resources.qrc
    )
else()
    target_link_libraries(deepin-reader PRIVATE
        ${READER_LINK_LIBS}
        dl
        pthread
    )
    # 使用DTK变量而不是目标
    target_link_libraries(deepin-reader PRIVATE
        ${DtkWidget_LIBRARIES}
        ${DtkGui_LIBRARIES}
        ${DtkCore_LIBRARIES}
    )

    qt5_add_resources(reader_RESOURCES
        ${CMAKE_SOURCE_DIR}/resources/resources.qrc
    )
endif()

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

if (XPS_SUPPORT_ENABLED)
    install(FILES
        ${CMAKE_SOURCE_DIR}/assets/mimetype/xps.xml
        DESTINATION ${CMAKE_INSTALL_DATADIR}/mime/packages
    )
    message(STATUS ">>> XPS MIME type definition will be installed to ${CMAKE_INSTALL_DATADIR}/mime/packages")
    message(STATUS ">>> Note: After installation, please run 'sudo update-mime-database ${CMAKE_INSTALL_DATADIR}/mime' to update MIME database")
endif()

# 安装帮助文件
install(DIRECTORY
    ${CMAKE_SOURCE_DIR}/assets/deepin-reader
    DESTINATION ${CMAKE_INSTALL_DATADIR}/deepin-manual/manual-assets/application
)

# log viewer config
file(GLOB LOGCONFIG_FILES "${CMAKE_SOURCE_DIR}/assets/logconfig/*.json")
install(FILES ${LOGCONFIG_FILES} DESTINATION share/deepin-log-viewer/deepin-log.conf.d)

# 安装 DConfig 配置文件，1040及以下的默认DTK环境不支持DConfig
set(DCONFIG_APPID org.deepin.reader)
file(GLOB DCONFIG_FILES "${CMAKE_SOURCE_DIR}/assets/configs/*.json")
if(DEFINED DSG_DATA_DIR)
    if (DTK_VERSION_MAJOR EQUAL "6")
        dtk_add_config_meta_files(APPID ${DCONFIG_APPID} FILES ${DCONFIG_FILES})
    else()
        dconfig_meta_files(APPID ${DCONFIG_APPID} FILES ${DCONFIG_FILES})
    endif()
    message("-- DConfig is supported by DTK")
else()
    install(FILES ${DCONFIG_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/dsg/configs/${DCONFIG_APPID})
    message("-- DConfig is not supported by DTK")
endif()

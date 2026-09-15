# SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
#
# SPDX-License-Identifier: GPL-3.0-or-later

# [v4] Batch print unit tests switch (default OFF)
option(BATCH_PRINT_TESTS "Build batch print unit tests" OFF)

# Find Qt components needed by batch-print modules (DBus for notifications, PrintSupport for QPdfWriter)
pkg_check_modules(FREETYPE REQUIRED freetype2)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS DBus PrintSupport Sql)

# batchprint-core: CupsClient + NotifyClient + PrintSettings + ErrorMessages (no reader source dependency)
add_library(batchprint-core STATIC
    cupsclient.h cupsclient.cpp
    icupsapi.h
    notifyclient.h notifyclient.cpp
    printsettings.h printsettings.cpp
    errormessages.h errormessages.cpp
)

target_include_directories(batchprint-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(batchprint-core PUBLIC
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::DBus
)

# batchprint-convert: FormatConverter (depends on selected reader/document sources + deepin-pdfium)
# Explicitly list only the reader source files needed for document loading/rendering,
# NOT the entire reader GUI (browser/sidebar/widgets/uiframe).
set(BATCHPRINT_READER_SOURCES
    ${CMAKE_SOURCE_DIR}/reader/app/Global.cpp
    ${CMAKE_SOURCE_DIR}/reader/document/Model.cpp
    ${CMAKE_SOURCE_DIR}/reader/document/PDFModel.cpp
    ${CMAKE_SOURCE_DIR}/reader/document/DjVuModel.cpp
    ${CMAKE_SOURCE_DIR}/reader/document/XpsDocumentAdapter.cpp
    ${CMAKE_SOURCE_DIR}/reader/document/XpsTextExtractor.cpp
)

add_library(batchprint-convert STATIC
    formatconverter.h formatconverter.cpp
    ${BATCHPRINT_READER_SOURCES}
)

set_target_properties(batchprint-convert PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
)

target_include_directories(batchprint-convert PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/reader
    ${CMAKE_SOURCE_DIR}/reader/app
    ${CMAKE_SOURCE_DIR}/reader/document
    ${PDFIUM_INCLUDE_DIRS}
    $<$<BOOL:${XPS_SUPPORT_ENABLED}>:${XPS_DEPS_INCLUDE_DIRS}>
)

target_compile_definitions(batchprint-convert PRIVATE
    INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX}"
    INSTALL_LIBDIR="${CMAKE_INSTALL_LIBDIR}"
    APP_VERSION="1.0.0"
)

target_link_libraries(batchprint-convert PUBLIC
    batchprint-core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::Svg
    Qt${QT_VERSION_MAJOR}::Concurrent
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Xml
    ${DDJVU_LIBRARIES}
    ${LIBJPEG_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    $<$<BOOL:${XPS_SUPPORT_ENABLED}>:${XPS_DEPS_LIBRARIES}>
)

# DTK linking
if(DTK_USE_TARGETS)
    target_link_libraries(batchprint-convert PUBLIC
        Dtk${DTK_VERSION_MAJOR}::Widget
        Dtk${DTK_VERSION_MAJOR}::Gui
        Dtk${DTK_VERSION_MAJOR}::Core
    )
endif()

if (QT_VERSION_MAJOR MATCHES 6)
    target_link_libraries(batchprint-convert PUBLIC Qt${QT_VERSION_MAJOR}::Core5Compat)
endif()

if (USE_PDFIUM_BUNDLE)
    target_link_libraries(batchprint-convert PUBLIC deepin-pdfium-reader)
else()
    target_link_libraries(batchprint-convert PUBLIC PkgConfig::Deepin-pdfium)
endif()

if (XPS_SUPPORT_ENABLED)
    target_compile_definitions(batchprint-convert PUBLIC XPS_SUPPORT_ENABLED)
    target_compile_options(batchprint-convert PUBLIC ${XPS_DEPS_CFLAGS_OTHER})
endif()

# Executable
add_executable(deepin-reader-batchprint
    main.cpp
    batchprintapp.h batchprintapp.cpp
)

target_link_libraries(deepin-reader-batchprint PRIVATE
    batchprint-core
    batchprint-convert
)

install(TARGETS deepin-reader-batchprint
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if (BATCH_PRINT_TESTS)
    add_subdirectory(tests)
endif()

# Batch print unit tests
if (BATCH_PRINT_TESTS)
    enable_testing()
endif()

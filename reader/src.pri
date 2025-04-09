INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/uiframe
INCLUDEPATH += $$PWD/widgets
INCLUDEPATH += $$PWD/sidebar
INCLUDEPATH += $$PWD/browser
INCLUDEPATH += $$PWD/document

include ($$PWD/app/app.pri)
include ($$PWD/browser/browser.pri)
include ($$PWD/sidebar/sidebar.pri)
include ($$PWD/widgets/widgets.pri)
include ($$PWD/document/document.pri)
include ($$PWD/uiframe/uiframe.pri)

SOURCES += \
    $$PWD/Application.cpp \
    $$PWD/MainWindow.cpp \
    $$PWD/load_libs.c

HEADERS +=\
    $$PWD/Application.h\
    $$PWD/MainWindow.h \
    $$PWD/load_libs.h

RESOURCES    += $$PWD/../resources/resources.qrc

#link
#由于自动化构建暂时无法自动下载sub module,目前手动内置第三方库
INCLUDEPATH += $$PWD/../3rdparty/deepin-pdfium/include
LIBS += -L$$PWD/../3rdparty/deepin-pdfium/lib -ldeepin-pdfium-reader -ldl

# 基础 Qt 模块
equals(QT_MAJOR_VERSION, 6) {
    QT += core gui widgets network dbus sql svg webchannel webenginewidgets concurrent xml core5compat
    
    # Qt6 specific configurations
    PKGCONFIG += dtk6widget dtk6gui dtk6core

    # Qt6 lrelease configuration
    QMAKE_LRELEASE = /usr/lib/qt6/bin/lrelease
    
} else {
    QT += core gui widgets network sql dbus svg webchannel webenginewidgets concurrent xml
    
    # Qt5 specific configurations 
    PKGCONFIG += dtkwidget dtkgui dtkcore
    
    QMAKE_LRELEASE = lrelease
}

# lrelease 编译规则
lrelease.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
lrelease.input = TRANSLATIONS
lrelease.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
lrelease.CONFIG += no_link target_predeps
QMAKE_EXTRA_COMPILERS += lrelease

CONFIG += c++11 link_pkgconfig

QMAKE_CXXFLAGS += -g -Wall
QMAKE_LFLAGS += -g -Wall

INCLUDEPATH += $$PWD

# 打印 Qt 版本信息
message("Qt Version: $$QT_VERSION")
message("Qt Major Version: $$QT_MAJOR_VERSION")
message("Qt Minor Version: $$QT_MINOR_VERSION")
message("Qt Patch Version: $$QT_PATCH_VERSION")



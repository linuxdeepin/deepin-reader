#config
QT += core gui sql printsupport dbus testlib widgets

CONFIG += c++11 link_pkgconfig resources_big testcase no_testcase_installs

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0

QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0

PKGCONFIG += ddjvuapi dtkwidget

CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

LIBS += -lgtest

#target
TARGET = test-deepin-reader

QMAKE_RPATHDIR += $$OUT_PWD/../3rdparty/deepin-pdfium/lib

TEMPLATE = app

#code
SOURCES += \
    sidebar/ut_sheetsidebar.cpp \
    uiframe/ut_uiframe.cpp \
    ut_main.cpp \
    ut_mainwindow.cpp \
    widgets/ut_widgets.cpp \
    app/ut_app.cpp \
    ut_application.cpp \
    browser/ut_browser.cpp \
    document/ut_document.cpp

HEADERS +=\
    sidebar/ut_sheetsidebar.h \
    uiframe/ut_uiframe.h \
    ut_defines.h \
    ut_mainwindow.h \
    widgets/ut_widgets.h \
    app/ut_app.h \
    ut_application.h \
    browser/ut_browser.h \
    document/ut_document.h

include($$PWD/../reader/src.pri)

RESOURCES += \
    files.qrc

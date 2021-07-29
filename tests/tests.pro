#config
QT += core gui sql printsupport dbus testlib widgets

###安全漏洞检测
#QMAKE_CXX += -g -fsanitize=undefined,address -O2
#QMAKE_CXXFLAGS += -g -fsanitize=undefined,address -O2
#QMAKE_LFLAGS += -g -fsanitize=undefined,address -O2

CONFIG += c++11 link_pkgconfig resources_big testcase no_testcase_installs

#访问私有方法 -fno-access-control
QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -fno-access-control -O0

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
    main.cpp \
    ut_application.cpp \
    app/ut_app.cpp \
    uiframe/ut_uiframe.cpp \
    ut_mainwindow.cpp \
    widgets/ut_widgets.cpp \
    widgets/ut_AttrScrollWidget.cpp \
    widgets/ut_BaseWidget.cpp \
    widgets/ut_ColorWidgetAction.cpp \
    widgets/ut_FileAttrWidget.cpp \
    widgets/ut_FindWidget.cpp \
    widgets/ut_HandleMenu.cpp \
    widgets/ut_PagingWidget.cpp \
    widgets/ut_ProgressDialog.cpp \
    widgets/ut_SaveDialog.cpp \
    widgets/ut_ScaleMenu.cpp \
    widgets/ut_ScaleWidget.cpp \
    widgets/ut_SecurityDialog.cpp \
    widgets/ut_ShortCutShow.cpp \
    widgets/ut_SlideWidget.cpp \
    sidebar/ut_sheetsidebar.cpp \
    browser/ut_browser.cpp \
    browser/ut_sheetbrowser.cpp \
    document/ut_document.cpp

HEADERS +=\
    ut_defines.h \
    ut_application.h \
    app/ut_app.h \
    uiframe/ut_uiframe.h \
    ut_mainwindow.h \
    widgets/ut_widgets.h \
    sidebar/ut_sheetsidebar.h \
    browser/ut_browser.h \
    document/ut_document.h

include($$PWD/../reader/src.pri)

RESOURCES += \
    files.qrc

#stub头文件路径
INCLUDEPATH += $$PWD/include/gtest

#安全测试选项
if(contains(DEFINES, CMAKE_SAFETYTEST_ARG_ON)){
    QMAKE_CFLAGS += -g -fsanitize=address -O2
    QMAKE_LFLAGS += -g -fsanitize=address -O2
    QMAKE_CXXFLAGS += -g -fsanitize=address -O2
}

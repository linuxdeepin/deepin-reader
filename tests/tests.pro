#config
QT += core gui sql printsupport dbus testlib widgets

###安全漏洞检测
#QMAKE_CXX += -g -fsanitize=undefined,address -O2
#QMAKE_CXXFLAGS += -g -fsanitize=undefined,address -O2
#QMAKE_LFLAGS += -g -fsanitize=undefined,address -O2

CONFIG += c++11 link_pkgconfig resources_big testcase no_testcase_installs

#访问私有方法 -fno-access-control
QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -fno-access-control -O0 -fno-inline

QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0

#安全编译参数
QMAKE_CFLAGS += -fstack-protector-strong -D_FORTITY_SOURCE=1 -z noexecstack -pie -fPIC -z lazy
QMAKE_CXXFLAGS += -fstack-protector-strong -D_FORTITY_SOURCE=1 -z noexecstack -pie -fPIC -z lazy

PKGCONFIG += ddjvuapi dtkwidget

CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

#定义宏定义给UT代码使用（UT工程路径）
DEFINES += UTSOURCEDIR=\"\\\"$$PWD\\\"\"

LIBS += -lgtest

#target
TARGET = test-deepin-reader

QMAKE_RPATHDIR += $$OUT_PWD/../3rdparty/deepin-pdfium/lib

TEMPLATE = app

#code
SOURCES += \
    main.cpp \
    ut_mainwindow.cpp \
    ut_common.cpp \
    ut_application.cpp \
    app/ut_Database.cpp \
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
    widgets/ut_TipsWidget.cpp \
    uiframe/ut_Central.cpp \
    sidebar/ut_BookMarkDelegate.cpp \
    sidebar/ut_BookMarkWidget.cpp \
    sidebar/ut_CatalogTreeView.cpp \
    sidebar/ut_SideBarImageListview.cpp \
    sidebar/ut_SideBarImageViewModel.cpp \
    browser/ut_sheetbrowser.cpp \
    document/ut_model.cpp \
    document/ut_pdfmodel.cpp \
    document/ut_djvumodel.cpp \
    browser/ut_browserannotation.cpp \
    browser/ut_browsermagnifier.cpp \
    browser/ut_browsermenu.cpp \
    browser/ut_browserpage.cpp \
    browser/ut_browserword.cpp \
    browser/ut_pagerenderthread.cpp \
    app/ut_dbusobject.cpp \
    app/ut_global.cpp \
    app/ut_debugtimemanager.cpp \
    app/ut_utils.cpp \
    uiframe/ut_centraldocpage.cpp \
    uiframe/ut_docsheet.cpp

HEADERS +=\
    ut_defines.h \
    ut_common.h

include($$PWD/../reader/src.pri)

RESOURCES += \
    files.qrc

#stub头文件路径
INCLUDEPATH += $$PWD/include/gtest

#安全测试选项
if(contains(DEFINES, CMAKE_SAFETYTEST_ARG_ON)){
    QMAKE_CFLAGS += -g -fsanitize=undefined,address -O2
    QMAKE_LFLAGS += -g -fsanitize=undefined,address -O2
    QMAKE_CXXFLAGS += -g -fsanitize=undefined,address -O2
}

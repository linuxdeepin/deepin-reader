QT += core gui sql printsupport dbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test-deepin-reader

TEMPLATE = app

CONFIG += c++11 link_pkgconfig testcase no_testcase_install

PKGCONFIG += ddjvuapi dtkwidget

SRCPWD=$$PWD/../src    #用于被单元测试方便的复用

3RDPARTTPATH = $${SRCPWD}/../3rdparty

INCLUDEPATH += $${SRCPWD}
INCLUDEPATH += $${SRCPWD}/uiframe

LIBS += -lopenjp2 -llcms2 -lfreetype -lgtest

#由于自动化构建暂时无法自动下载sub module,目前手动内置第三方库
CONFIG(release, debug|release) {
INCLUDEPATH += $${3RDPARTTPATH}/deepin-pdfium/include
LIBS += -L$${3RDPARTTPATH}/deepin-pdfium/lib -ldeepin-pdfium
}

CONFIG(debug, debug|release) {
INCLUDEPATH += $${3RDPARTTPATH}/deepdf/include
LIBS += -L$${3RDPARTTPATH}/deepdf/lib -ldeepdf
}

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0


include ($$SRCPWD/app/app.pri)
include ($$SRCPWD/browser/browser.pri)
include ($$SRCPWD/sidebar/sidebar.pri)
include ($$SRCPWD/widgets/widgets.pri)
include ($$SRCPWD/document/document.pri)

SOURCES += \
    $$SRCPWD/Application.cpp \
    sidebar/ut_sheetsidebar.cpp \
    uiframe/ut_uiframe.cpp \
    ut_main.cpp \
    $$SRCPWD/MainWindow.cpp \
    $$SRCPWD/uiframe/TitleMenu.cpp \
    $$SRCPWD/uiframe/TitleWidget.cpp \
    $$SRCPWD/uiframe/Central.cpp \
    $$SRCPWD/uiframe/CentralNavPage.cpp \
    $$SRCPWD/uiframe/CentralDocPage.cpp \
    $$SRCPWD/uiframe/DocTabBar.cpp \
    $$SRCPWD/uiframe/DocSheet.cpp \
    ut_mainwindow.cpp \
    widgets/ut_widgets.cpp \
    app/ut_app.cpp \
    ut_application.cpp \
    browser/ut_browser.cpp \
    document/ut_document.cpp

HEADERS +=\
    $$SRCPWD/Application.h\
    $$SRCPWD/MainWindow.h \
    $$SRCPWD/uiframe/TitleWidget.h \
    $$SRCPWD/uiframe/TitleMenu.h \
    $$SRCPWD/uiframe/Central.h \
    $$SRCPWD/uiframe/CentralNavPage.h \
    $$SRCPWD/uiframe/CentralDocPage.h \
    $$SRCPWD/uiframe/DocTabBar.h \
    $$SRCPWD/uiframe/DocSheet.h \
    sidebar/ut_sheetsidebar.h \
    uiframe/ut_uiframe.h \
    ut_defines.h \
    ut_mainwindow.h \
    widgets/ut_widgets.h \
    app/ut_app.h \
    ut_application.h \
    browser/ut_browser.h \
    document/ut_document.h

RESOURCES +=         \
    $$SRCPWD/../resources/resources.qrc

target.path   = /usr/bin

desktop.path  = /usr/share/applications
desktop.files = $$SRCPWD/deepin-reader.desktop

poppler.path = /usr/lib
poppler.files = $${3RDPARTTPATH}/lib/*.so*

icon.path = /usr/share/icons/hicolor/scalable/apps
icon.files = $$SRCPWD/deepin-reader.svg

INSTALLS += target desktop icon poppler

CONFIG(release, debug|release) {
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    TRANSLATIONFILES= $$files($$SRCPWD/../translations/*.ts)
    for(tsfile, TRANSLATIONFILES) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
    #将qm文件添加到安装包
    dtk_translations.path = /usr/share/$$TARGET/translations
    dtk_translations.files = $$SRCPWD/../translations/*.qm
    INSTALLS += dtk_translations
}

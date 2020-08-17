include ($$PWD/gtest_dependency.pri)

QT += core gui sql printsupport dbus network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-reader-test

TEMPLATE = app

CONFIG += c++11 link_pkgconfig

PKGCONFIG += ddjvuapi dtkwidget

SRCPWD=$$PWD/../../src    #用于被单元测试方便的复用
#QMAKE_CXXFLAGS += --coverage #告诉g++我们要做coverage
#LIBS += -lgcov

QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage -O0
QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage  -O0

3RDPARTTPATH = $$SRCPWD/../3rdparty
INCLUDEPATH += $$SRCPWD/uiframe
INCLUDEPATH += $${3RDPARTTPATH}/include
INCLUDEPATH += $$SRCPWD

LIBS += -L"$${3RDPARTTPATH}/lib" -ldeepin-poppler-qt -ldeepin-poppler
!system(mkdir -p $${3RDPARTTPATH}/output && cd $${3RDPARTTPATH}/output && cmake $${3RDPARTTPATH}/poppler-0.89.0 && make){
    error("Build deepin-poppler library failed.")
}
QMAKE_RPATHDIR += usr/lib/deepin-reader

include ($$SRCPWD/app/app.pri)
include ($$SRCPWD/browser/browser.pri)
include ($$SRCPWD/sidebar/sidebar.pri)
include ($$SRCPWD/widgets/widgets.pri)
include ($$SRCPWD/document/document.pri)

SOURCES += \
    $$SRCPWD/Application.cpp \
    main.cpp \
    $$SRCPWD/MainWindow.cpp \
    $$SRCPWD/uiframe/TitleMenu.cpp \
    $$SRCPWD/uiframe/TitleWidget.cpp \
    $$SRCPWD/uiframe/Central.cpp \
    $$SRCPWD/uiframe/CentralNavPage.cpp \
    $$SRCPWD/uiframe/CentralDocPage.cpp \
    $$SRCPWD/uiframe/DocTabBar.cpp \
    $$SRCPWD/uiframe/DocSheet.cpp

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
    tst_siderbar.h \
    tst_mainwindow.h

RESOURCES +=         \
    $$SRCPWD/../resources/resources.qrc

TRANSLATIONS += \
    $$SRCPWD/../translations/deepin-reader_en_US.ts\
    $$SRCPWD/../translations/deepin-reader_zh_CN.ts

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

QT += core gui sql printsupport dbus network xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-reader

TEMPLATE = app

CONFIG += c++11 link_pkgconfig

PKGCONFIG += ddjvuapi dtkwidget

INCLUDEPATH += $$PWD/uiframe
INCLUDEPATH += $$PWD/../3rdparty/include

LIBS += -L"$$PWD/../3rdparty/lib" -ldpoppler-qt -ldpoppler
!system(cd $$PWD/../3rdparty/output && cmake $$PWD/../3rdparty/poppler-0.89.0 && make){
    error("Build dpoppler library failed.")
}

include (app/app.pri)
include (pdfControl/pdfControl.pri)
include (pdflControl/pdflControl.pri)
include (utils/Utils.pri)
include (widgets/widgets.pri)
include (djvuControl/djvuControl.pri)
include (document/document.pri)
include (lpreviewControl/lpreviewControl.pri)

SOURCES += \
    Application.cpp \
        main.cpp \
    MainWindow.cpp \
    uiframe/TitleMenu.cpp \
    uiframe/TitleWidget.cpp \
    uiframe/Central.cpp \
    uiframe/CentralNavPage.cpp \
    uiframe/CentralDocPage.cpp \
    uiframe/DocTabBar.cpp \
    uiframe/DocSheet.cpp

HEADERS +=\
    Application.h \
    MainWindow.h \
    uiframe/TitleWidget.h \
    uiframe/TitleMenu.h \
    uiframe/Central.h \
    uiframe/CentralNavPage.h \
    uiframe/CentralDocPage.h \
    uiframe/DocTabBar.h \
    uiframe/DocSheet.h

RESOURCES +=         \
    ../resources/resources.qrc

TRANSLATIONS += \
    ../translations/deepin-reader_en_US.ts\
    ../translations/deepin-reader_zh_CN.ts

target.path   = /usr/bin
desktop.path  = /usr/share/applications
desktop.files = $$PWD/deepin-reader.desktop
INSTALLS += target desktop

CONFIG(release, debug|release) {
    #遍历目录中的ts文件，调用lrelease将其生成为qm文件
    TRANSLATIONFILES= $$files($$PWD/../translations/*.ts)
    for(tsfile, TRANSLATIONFILES) {
        qmfile = $$replace(tsfile, .ts$, .qm)
        system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
    }
    #将qm文件添加到安装包
    dtk_translations.path = /usr/share/$$TARGET/translations
    dtk_translations.files = $$PWD/../translations/*.qm
    INSTALLS += dtk_translations
}

unix{
    d_poppler.path = /usr/lib
    d_poppler.files = $$PWD/../3rdparty/lib/*.so*
    INSTALLS += d_poppler
}


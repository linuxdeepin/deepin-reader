QT += core gui svg sql printsupport
QT += dtkwidget
QT += xml
#QT += KCodecs

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

#private
QT       += gui-private

TARGET = deepin-reader
TEMPLATE = app
CONFIG += c++11 link_pkgconfig

include (app/app.pri)
include (business/business.pri)
include (CustomControl/CustomControl.pri)
include (pdfControl/pdfControl.pri)
include (utils/utils.pri)
include (widgets/widgets.pri)
include (djvuControl/djvuControl.pri)
include (document/document.pri)
include (lpreviewControl//lpreviewControl.pri)

INCLUDEPATH += $$PWD/uiframe

SOURCES += \
        main.cpp \
    application.cpp\
    MainWindow.cpp \
    uiframe/TitleMenu.cpp \
    uiframe/TitleWidget.cpp \
    uiframe/Central.cpp \
    uiframe/CentralNavPage.cpp \
    uiframe/CentralDocPage.cpp \
    uiframe/DocTabBar.cpp \
    uiframe/DocSheet.cpp

RESOURCES +=         \
    ../resources/resources.qrc

HEADERS +=\
    application.h \
    MainWindow.h \
    uiframe/TitleWidget.h \
    uiframe/TitleMenu.h \
    uiframe/Central.h \
    uiframe/CentralNavPage.h \
    uiframe/CentralDocPage.h \
    uiframe/DocTabBar.h \
    uiframe/DocSheet.h

APPICONDIR = $$PREFIX/share/icons/deepin/apps/scalable

isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-reader

target.path = $$INSTROOT$$BINDIR

desktop.path = $$INSTROOT$$APPDIR
desktop.files = $$PWD/deepin-reader.desktop

icon_files.path = /usr/share/icons/hicolor/scalable/apps
icon_files.files = $$PWD/deepin-reader.svg

INSTALLS += target desktop icon_files

TRANSLATIONS += \
    ../translations/deepin-reader_en_US.ts\
    ../translations/deepin-reader_zh_CN.ts

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

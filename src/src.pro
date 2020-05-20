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

#CONFIG += precompile_header
#PRECOMPILED_HEADER = stable.h

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
    icons.qrc \
    image.qrc

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
icon_files.files = $$PWD/icons/deepin/builtin/deepin-reader.svg

INSTALLS += target desktop icon_files


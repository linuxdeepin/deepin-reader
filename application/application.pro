QT += core gui svg sql printsupport
QT += dtkwidget
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin-reader
TEMPLATE = app
CONFIG += c++11 link_pkgconfig

include (frame/frame.pri)
include (utils/utils.pri)
include (controller/controller.pri)
include (pdfControl/pdfControl.pri)
include (docview/docview.pri)
include (subjectObserver/subjectObserver.pri)
include (mainShow/mainShow.pri)
include (translator/translator.pri)

SOURCES += \
        main.cpp \
    application.cpp\

RESOURCES +=         resources.qrc

HEADERS +=\
    application.h


DISTFILES += \
    themes/solarized_light.theme \
    themes/atom_dark.theme \
    themes/deepin_dark.theme \
    themes/solarized_dark.theme \
    themes/deepin.theme

APPICONDIR = $$PREFIX/share/icons/deepin/apps/scalable

#app_icon.path = /usr/share/icons/hicolor/scalable/apps
#app_icon.files = $$PWD/resources/image/logo/deepin-reader.svg


isEmpty(BINDIR):BINDIR=/usr/bin
isEmpty(APPDIR):APPDIR=/usr/share/applications
isEmpty(DSRDIR):DSRDIR=/usr/share/deepin-reader

target.path = $$INSTROOT$$BINDIR
desktop.path = $$INSTROOT$$APPDIR
desktop.files = $$PWD/resources/deepin-reader.desktop

icon_files.path = /usr/share/icons/hicolor/scalable/apps
icon_files.files = $$PWD/resources/image/logo/deepin-reader.svg

INSTALLS += target desktop icon_files




#-------------------------------------------------
#
# Project created by QtCreator 2020-02-21T16:36:20
#
#-------------------------------------------------

QT       -= gui

TARGET = ModelService
TEMPLATE = lib

DEFINES += MODELSERVICE_LIBRARY

SOURCES += \
        ModelService.cpp \
    NotifySubject.cpp

HEADERS += \
        ModelService.h \
        modelservice_global.h \ 
    IObserver.h \
    ISubject.h \
    MsgHeader.h \
    Singleton.h \
    ModuleHeader.h \
    NotifySubject.h \
    WidgetHeader.h


QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_NAME = ModelService
QMAKE_PKGCONFIG_DESCRIPTION = ModelService Manager
QMAKE_PKGCONFIG_INCDIR = $$includes.path


unix {
    target.path = /usr/lib
    INSTALLS += target
}

#-------------------------------------------------
#
# Project created by QtCreator 2020-02-20T15:12:58
#
#-------------------------------------------------

QT       += sql

QT       -= gui

TARGET = DBService
TEMPLATE = lib

DEFINES += DBSERVICE_LIBRARY

SOURCES += \
        DBService.cpp \
    DBFactory.cpp \
    BookMarkDB.cpp \
    HistroyDB.cpp \
    FileDataModel.cpp

HEADERS += \
        DBService.h \
        dbservice_global.h \ 
    DBFactory.h \
    BookMarkDB.h \
    HistroyDB.h \
    FileDataModel.h

QMAKE_PKGCONFIG_LIBDIR = $$target.path
QMAKE_PKGCONFIG_VERSION = $$VERSION
QMAKE_PKGCONFIG_DESTDIR = pkgconfig
QMAKE_PKGCONFIG_NAME = DBService
QMAKE_PKGCONFIG_DESCRIPTION = DBService Manager
QMAKE_PKGCONFIG_INCDIR = $$includes.path

unix{
    target.path = /usr/lib
    INSTALLS += target
}

QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin_reader
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

SOURCES += \
        main.cpp \
    tabbar.cpp \
    utils.cpp \
    window.cpp \
    settings.cpp


RESOURCES +=         resources.qrc

HEADERS += \
    tabbar.h \
    utils.h \
    window.h \
    settings.h

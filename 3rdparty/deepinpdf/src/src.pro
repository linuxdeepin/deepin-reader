TARGET = $$PWD/../lib/deepinpdf

DEFINES += __QT__ \
    OPJ_STATIC \
    PNG_PREFIX \
    PNG_USE_READ_MACROS

TEMPLATE = lib

CONFIG += c++11

QT = core-private core gui

include($$PWD/../3rdparty/pdfium.pri)

HEADERS += \
    $$PWD/dpdfiumglobal.h \
    $$PWD/dpdfium.h \
    $$PWD/dpdfiumpage.h
SOURCES += \
    $$PWD/dpdfiumglobal.cpp \
    $$PWD/dpdfium.cpp \
    $$PWD/dpdfiumpage.cpp

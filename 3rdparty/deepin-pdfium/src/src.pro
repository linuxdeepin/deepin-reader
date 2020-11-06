TARGET = $$PWD/../lib/deepin-pdfium

TEMPLATE = lib

CONFIG += c++14

QT = core-private core gui

include($$PWD/3rdparty/pdfium/pdfium.pri)

INCLUDEPATH += $$PWD/../include

public_headers += \
    $$PWD/../include/dpdfglobal.h \
    $$PWD/../include/dpdfdoc.h \
    $$PWD/../include/dpdfpage.h \
    $$PWD/../include/dpdfannot.h

HEADERS += $$public_headers

SOURCES += \
    $$PWD/dpdfglobal.cpp \
    $$PWD/dpdfdoc.cpp \
    $$PWD/dpdfpage.cpp \
    $$PWD/dpdfannot.cpp

target.path  = /usr/lib

INSTALLS += target

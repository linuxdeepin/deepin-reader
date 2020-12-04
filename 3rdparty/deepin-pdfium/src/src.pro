TARGET = $$PWD/../lib/deepin-pdfium

TEMPLATE = lib

CONFIG += c++14

DEFINES += BUILD_DEEPDF_LIB

QT += core-private core gui

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

include($$PWD/3rdparty/pdfium/pdfium.pri)

INCLUDEPATH += $$PWD/../include

INCLUDEPATH += /usr/include/chardet

LIBS += -lchardet

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

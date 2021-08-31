TARGET = $$PWD/../lib/deepin-pdfium

TEMPLATE = lib

CONFIG += c++14

###安全漏洞检测
#QMAKE_CXX += -g -fsanitize=undefined,address -O2
#QMAKE_CXXFLAGS += -g -fsanitize=undefined,address -O2
#QMAKE_LFLAGS += -g -fsanitize=undefined,address -O2

#获取当前系统页大小
PageSize = $$system(getconf PAGESIZE)
DEFINES += "SYSTEMPAGESIZE=$$PageSize"
message("DEFINES: "$$DEFINES)

DEFINES += BUILD_DEEPDF_LIB

QT += core-private core gui

QMAKE_CXXFLAGS_WARN_ON += -Wno-unused-parameter

include($$PWD/3rdparty/pdfium/pdfium.pri)

INCLUDEPATH += $$PWD/../include

INCLUDEPATH += /usr/include/chardet

LIBS += -lchardet -ljpeg -licuuc -lz

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

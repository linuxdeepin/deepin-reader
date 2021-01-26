#config
QT += core gui sql printsupport dbus widgets

PKGCONFIG += ddjvuapi dtkwidget

QMAKE_CXXFLAGS += -fPIE

QMAKE_LFLAGS += -pie

VERSION=$(DEB_VERSION_UPSTREAM)

contains(QMAKE_HOST.arch, mips64):{
    QMAKE_CXXFLAGS += "-O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi"
}

CONFIG += c++11 link_pkgconfig

#target
TARGET = deepin-reader

TEMPLATE = app

#DEFINES += PERF_ON

#Install
target.path   = /usr/bin

desktop.path  = /usr/share/applications

desktop.files = $$PWD/deepin-reader.desktop

icon.path = /usr/share/icons/hicolor/scalable/apps

icon.files = $$PWD/deepin-reader.svg

manual.files = $$PWD/assets/deepin-reader

manual.path= /usr/share/deepin-manual/manual-assets/application/

INSTALLS += target desktop icon manual

#translate
TRANSLATIONS += $$PWD/../translations/deepin-reader.ts

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

#code
SOURCES += \
    $$PWD/main.cpp \

include($$PWD/src.pri)

include($$PWD/src.pri)
INCLUDEPATH += $$INCPATHS

#config
# 基础 Qt 模块
equals(QT_MAJOR_VERSION, 6) {
    QT += core gui widgets network dbus sql svg webchannel webenginewidgets concurrent xml core5compat

    # Qt6 specific configurations
    PKGCONFIG += dtk6widget dtk6gui dtk6core

    # Qt6 lrelease configuration
    QMAKE_LRELEASE = /usr/lib/qt6/bin/lrelease

} else {
    QT += core gui widgets network sql dbus svg webchannel webenginewidgets concurrent xml

    # Qt5 specific configurations
    PKGCONFIG += dtkwidget dtkgui dtkcore

    QMAKE_LRELEASE = lrelease
}

# # 检查 Qt 版本
# greaterThan(QT_MAJOR_VERSION, 5): {
#     # 如果 Qt 版本是 6
#     CONFIG += build_with_qt6
#     DTK_VERSION_MAJOR = 6
# } else {
#     # 如果不是 Qt 6
#     DTK_VERSION_MAJOR = ""
# }

message("Using Qt version: $$QT_MAJOR_VERSION")

isEmpty(PREFIX) {
 PREFIX = /usr
}
DEFINES += QMAKE_INSTALL_PREFIX=\"\\\"$$PREFIX\\\"\"

###安全漏洞检测
#QMAKE_CXX += -g -fsanitize=undefined,address -O2
#QMAKE_CXXFLAGS += -g -fsanitize=undefined,address -O2
#QMAKE_LFLAGS += -g -fsanitize=undefined,address -O2

#安全编译参数
QMAKE_CFLAGS += -fstack-protector-strong -D_FORTITY_SOURCE=1 -z noexecstack -pie -fPIC -z lazy -D_FORTIFY_SOURCE=2 -O2
QMAKE_CXXFLAGS += -fstack-protector-strong -D_FORTITY_SOURCE=1 -z noexecstack -pie -fPIC -z lazy -D_FORTIFY_SOURCE=2 -O2

QMAKE_CXXFLAGS += -fPIE

QMAKE_LFLAGS += -pie -Wl,-z,relro -Wl,-z,now

DEFINES += APP_VERSION=\\\"$$VERSION\\\"
message("APP_VERSION: "$$VERSION)

contains(QMAKE_HOST.arch, mips64):{
    QMAKE_CXXFLAGS += "-O3 -ftree-vectorize -march=loongson3a -mhard-float -mno-micromips -mno-mips16 -flax-vector-conversions -mloongson-ext2 -mloongson-mmi"
}

#代码覆盖率开关
if(contains(DEFINES, CMAKE_COVERAGE_ARG_ON)){
    QMAKE_CFLAGS += -g -Wall -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -g -Wall -fprofile-arcs -ftest-coverage
    QMAKE_CXXFLAGS += -g -Wall -fprofile-arcs -ftest-coverage
}

CONFIG += c++11 link_pkgconfig

TARGET = deepin-reader

TEMPLATE = app

#DEFINES += PERF_ON

#Install
target.path   = $$PREFIX/bin

desktop.path  = $$PREFIX/share/applications

desktop.files = $$PWD/deepin-reader.desktop

icon.path = $$PREFIX/share/icons/hicolor/scalable/apps

icon.files = $$PWD/deepin-reader.svg

manual.path = $$PREFIX/share/deepin-manual/manual-assets/application

manual.files = $$PWD/../assets/*

INSTALLS += target desktop icon manual

# #translate
# TRANSLATIONS += $$PWD/../translations/deepin-reader.ts

# CONFIG(release, debug|release) {
#     #遍历目录中的ts文件，调用lrelease将其生成为qm文件
#     TRANSLATIONFILES= $$files($$PWD/../translations/*.ts)
#     for(tsfile, TRANSLATIONFILES) {
#         qmfile = $$replace(tsfile, .ts$, .qm)
#         system(lrelease $$tsfile -qm $$qmfile) | error("Failed to lrelease")
#     }
#     #将qm文件添加到安装包
#     dtk_translations.path = $$PREFIX/share/$$TARGET/translations
#     dtk_translations.files = $$PWD/../translations/*.qm
#     INSTALLS += dtk_translations
# }
# 添加库
LIBS += -ldjvulibre -ljpeg 
#code
SOURCES += \
    $$PWD/main.cpp \


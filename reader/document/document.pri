HEADERS += \
    $$PWD/Model.h \
    $$PWD/PDFModel.h \
    $$PWD/DjVuModel.h

# XPS支持文件（条件包含）
# 注意：宏与链接库必须在此定义。subdirs 顶层的 DEFINES 不会传递到子工程，
# 而本文件所在工程由 CONFIG+=xps_support 激活（命令行 CONFIG 会随 qmake 递归传入）
xps_support {
    DEFINES += XPS_SUPPORT_ENABLED
    PKGCONFIG += libgxps cairo glib-2.0 gobject-2.0 freetype2
    HEADERS += $$PWD/XpsDocumentAdapter.h
    HEADERS += $$PWD/XpsTextExtractor.h
    SOURCES += $$PWD/XpsDocumentAdapter.cpp
    SOURCES += $$PWD/XpsTextExtractor.cpp
}

SOURCES += \
    $$PWD/PDFModel.cpp \
    $$PWD/DjVuModel.cpp \
    $$PWD/Model.cpp

INCLUDEPATH += $$PWD

DEFINES += INSTALL_PREFIX=\\\"$$PREFIX\\\"

HEADERS += \
    $$PWD/Model.h \
    $$PWD/PDFModel.h \
    $$PWD/DjVuModel.h

# XPS支持文件（条件包含）
xps_support {
    HEADERS += $$PWD/XpsDocumentAdapter.h
    HEADERS += $$PWD/XpsTextExtractor.h
    SOURCES += $$PWD/XpsDocumentAdapter.cpp
    SOURCES += $$PWD/XpsTextExtractor.cpp
}

# OFD支持文件（条件包含，需要系统安装的 librofd-ffi-dev，见 debian/control）
ofd_support {
    HEADERS += $$PWD/OfdModel.h
    SOURCES += $$PWD/OfdModel.cpp
    DEFINES += OFD_SUPPORT_ENABLED
}

SOURCES += \
    $$PWD/PDFModel.cpp \
    $$PWD/DjVuModel.cpp \
    $$PWD/Model.cpp

INCLUDEPATH += $$PWD

DEFINES += INSTALL_PREFIX=\\\"$$PREFIX\\\"

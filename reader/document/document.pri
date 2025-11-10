HEADERS += \
    $$PWD/Model.h \
    $$PWD/PDFModel.h \
    $$PWD/DjVuModel.h

# XPS支持文件（条件包含）
xps_support {
    HEADERS += $$PWD/XpsDocumentAdapter.h
    SOURCES += $$PWD/XpsDocumentAdapter.cpp
}

SOURCES += \
    $$PWD/PDFModel.cpp \
    $$PWD/DjVuModel.cpp \
    $$PWD/Model.cpp

INCLUDEPATH += $$PWD

DEFINES += INSTALL_PREFIX=\\\"$$PREFIX\\\"

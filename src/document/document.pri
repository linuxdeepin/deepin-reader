HEADERS += \
    $$PWD/Model.h \
    $$PWD/PDFModel.h \
    $$PWD/DjVuModel.h

SOURCES += \
    $$PWD/PDFModel.cpp \
    $$PWD/DjVuModel.cpp

INCLUDEPATH += $$PWD

!without_pkgconfig {
    DEFINES += HAS_POPPLER_14
    DEFINES += HAS_POPPLER_18
    DEFINES += HAS_POPPLER_20
    DEFINES += HAS_POPPLER_22
    DEFINES += HAS_POPPLER_24
    DEFINES += HAS_POPPLER_26
    DEFINES += HAS_POPPLER_31
    DEFINES += HAS_POPPLER_35
}

HEADERS += \
    $$PWD/Model.h \
    $$PWD/PDFModel.h \
    $$PWD/DjVuModel.h

SOURCES += \
    $$PWD/PDFModel.cpp \
    $$PWD/DjVuModel.cpp


!without_pkgconfig {
    poppler_qt_pkg = poppler-qt$${QT_MAJOR_VERSION}

    system(pkg-config --atleast-version=0.14 $${poppler_qt_pkg}):DEFINES += HAS_POPPLER_14
    system(pkg-config --atleast-version=0.18 $${poppler_qt_pkg}):DEFINES += HAS_POPPLER_18
    system(pkg-config --atleast-version=0.20.1 $${poppler_qt_pkg}):DEFINES += HAS_POPPLER_20
    system(pkg-config --atleast-version=0.22 $${poppler_qt_pkg}):DEFINES += HAS_POPPLER_22
    system(pkg-config --atleast-version=0.24 $${poppler_qt_pkg}):DEFINES += HAS_POPPLER_24
    system(pkg-config --atleast-version=0.26 $${poppler_qt_pkg}):DEFINES += HAS_POPPLER_26
    system(pkg-config --atleast-version=0.31 $${poppler_qt_pkg}):DEFINES += HAS_POPPLER_31
    system(pkg-config --atleast-version=0.35 $${poppler_qt_pkg}):DEFINES += HAS_POPPLER_35

    CONFIG += link_pkgconfig
    PKGCONFIG += $${poppler_qt_pkg}
} else {
    DEFINES += $$PDF_PLUGIN_DEFINES
    INCLUDEPATH += $$PDF_PLUGIN_INCLUDEPATH
    LIBS += $$PDF_PLUGIN_LIBS
}

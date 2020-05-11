PKGCONFIG += x11 poppler-qt5 ddjvuapi libspectre
INCLUDEPATH+=/usr/include/KF5/KArchive
LIBS+=-lKF5Archive -luuid -ltiff

HEADERS += \
    $$PWD/controller/Annotation.h \
    $$PWD/controller/ProxyData.h \
    $$PWD/controller/ProxyFileDataModel.h \
    $$PWD/controller/ProxyMouseMove.h \
    $$PWD/controller/ProxyNotifyMsg.h \
    $$PWD/controller/ProxyViewDisplay.h \
    $$PWD/SheetBrowserPDF.h \
    $$PWD/SheetBrowserPDFPrivate.h \
    $$PWD/DocSheetPDF.h \
    $$PWD/docview/commonstruct.h \
    $$PWD/docview/docummentbase.h \
    $$PWD/docview/docummentfactory.h \
    $$PWD/docview/docummentproxy.h \
    $$PWD/docview/generatorclass.h \
    $$PWD/docview/pagebase.h \
    $$PWD/docview/publicfunc.h \
    $$PWD/docview/searchtask.h \
    $$PWD/docview/pdf/pagepdf.h \
    $$PWD/docview/pdf/docummentpdf.h

SOURCES += \
    $$PWD/controller/Annotation.cpp \
    $$PWD/controller/ProxyData.cpp \
    $$PWD/controller/ProxyFileDataModel.cpp \
    $$PWD/controller/ProxyMouseMove.cpp \
    $$PWD/controller/ProxyNotifyMsg.cpp \
    $$PWD/controller/ProxyViewDisplay.cpp \
    $$PWD/SheetBrowserPDF.cpp \
    $$PWD/SheetBrowserPDFPrivate.cpp \
    $$PWD/DocSheetPDF.cpp \
    $$PWD/docview/docummentbase.cpp \
    $$PWD/docview/docummentfactory.cpp \
    $$PWD/docview/docummentproxy.cpp \
    $$PWD/docview/generatorclass.cpp \
    $$PWD/docview/pagebase.cpp \
    $$PWD/docview/publicfunc.cpp \
    $$PWD/docview/searchtask.cpp \
    $$PWD/docview/pdf/pagepdf.cpp \
    $$PWD/docview/pdf/docummentpdf.cpp

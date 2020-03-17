
PKGCONFIG += x11 poppler-qt5 ddjvuapi libspectre
INCLUDEPATH+=/usr/include/KF5/KArchive
LIBS+=-lKF5Archive -luuid -ltiff
HEADERS += \
    $$PWD/generatorclass.h \
    $$PWD/pdf/ \
    $$PWD/pdf/docummentpdf.h \
    $$PWD/pdf/pagepdf.h \
    $$PWD/docummentbase.h \
    $$PWD/docummentfactory.h \
    $$PWD/docummentproxy.h \
    $$PWD/pagebase.h \
    $$PWD/publicfunc.h \
    $$PWD/commonstruct.h \
    $$PWD/djvu/docummentdjvu.h \
    $$PWD/djvu/pagedjvu.h \
#    $$PWD/tiff/pagetiff.h \
#    $$PWD/tiff/documentiff.h\
    $$PWD/ps/pageps.h \
    $$PWD/ps/docummentps.h \
    $$PWD/xps/docummentxps.h \
    $$PWD/xps/pagexps.h \
    $$PWD/xps/xpsapi.h \
    $$PWD/searchtask.h \
    $$PWD/bookmarkbutton.h \
    $$PWD/tiff/documentiff.h \
    $$PWD/tiff/pagetiff.h


SOURCES += \
    $$PWD/generatorclass.cpp \
    $$PWD/pdf/docummentpdf.cpp \
    $$PWD/pdf/pagepdf.cpp \
    $$PWD/docummentbase.cpp \
    $$PWD/docummentfactory.cpp \
    $$PWD/docummentproxy.cpp \
    $$PWD/pagebase.cpp \
    $$PWD/publicfunc.cpp \
    $$PWD/djvu/docummentdjvu.cpp \
    $$PWD/djvu/pagedjvu.cpp \
#    $$PWD/tiff/pagetiff.cpp \
#    $$PWD/tiff/documentiff.cpp \
    $$PWD/ps/pageps.cpp \
    $$PWD/ps/docummentps.cpp \
    $$PWD/xps/pagexps.cpp \
    $$PWD/xps/docummentxps.cpp \
    $$PWD/xps/xpsapi.cpp \
    $$PWD/searchtask.cpp \
    $$PWD/bookmarkbutton.cpp \
    $$PWD/tiff/documentiff.cpp \
    $$PWD/tiff/pagetiff.cpp

RESOURCES += \
    $$PWD/docviewtheme.qrc


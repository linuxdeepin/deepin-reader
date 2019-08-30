QT += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin_reader
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += dtkwidget

SOURCES += \
        main.cpp \
    tabbar.cpp \
    utils.cpp \
    window.cpp \
    settings.cpp \
    toolbar.cpp \
    widgets/MainOperationWidget.cpp \
    widgets/MainWidget.cpp \
    widgets/TitleWidget.cpp \
    widgets/HomeWidget.cpp \
    widgets/LeftSidebarWidget.cpp \
    widgets/FileViewWidget.cpp \
    widgets/MainShowSplitter.cpp \
    widgets/FileAttrWidget.cpp \
    data/DataManager.cpp \
    header/ThemeSubject.cpp \
    listWidget/PagingWidget.cpp \
    listWidget/NotesWidget.cpp \
    listWidget/BookMarkWidget.cpp \
    listWidget/ThumbnailItemWidget.cpp \
    listWidget/ThumbnailWidget.cpp \
    listWidget/BookMarkItemWidget.cpp \
    listWidget/NotesItemWidget.cpp \
    view/MagnifyingWidget.cpp


RESOURCES +=         resources.qrc

HEADERS += \
    tabbar.h \
    utils.h \
    window.h \
    settings.h \
    toolbar.h \
    widgets/MainOperationWidget.h \
    widgets/TitleWidget.h \
    widgets/MainWidget.h \
    widgets/HomeWidget.h \
    widgets/LeftSidebarWidget.h \
    widgets/FileViewWidget.h \
    widgets/MainShowSplitter.h \
    widgets/FileAttrWidget.h \
    data/DataManager.h \
    header/ISubject.h \
    header/IObserver.h \
    header/ThemeSubject.h \
    header/IThemeObserver.h \
    listWidget/ThumbnailItemWidget.h \
    listWidget/NotesWidget.h \
    listWidget/ThumbnailWidget.h \
    listWidget/PagingWidget.h \
    listWidget/BookMarkWidget.h \
    listWidget/BookMarkItemWidget.h \
    listWidget/NotesItemWidget.h \
    view/MagnifyingWidget.h

DISTFILES += \
    themes/solarized_light.theme \
    themes/atom_dark.theme \
    themes/deepin_dark.theme \
    themes/solarized_dark.theme \
    themes/deepin.theme

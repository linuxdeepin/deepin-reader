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
    widgets/MainOperationWidget.cpp \
    widgets/MainWidget.cpp \
    widgets/TitleWidget.cpp \
    widgets/MainShowDataWidget.cpp \
    widgets/HomeWidget.cpp \
    widgets/LeftSidebarWidget.cpp \
    toolbar.cpp \
    data/DataManager.cpp \
    widgets/FileAttrWidget.cpp \
    header/ThemeSubject.cpp \
    listWidget/PagingWidget.cpp \
    listWidget/NotesWidget.cpp \
    listWidget/BookMarkWidget.cpp \
    listWidget/ThumbnailItemWidget.cpp \
    listWidget/ThumbnailWidget.cpp \
    listWidget/BookMarkItemWidget.cpp \
    listWidget/NotesItemWidget.cpp


RESOURCES +=         resources.qrc

HEADERS += \
    tabbar.h \
    utils.h \
    window.h \
    settings.h \
    widgets/MainOperationWidget.h \
    widgets/TitleWidget.h \
    widgets/MainWidget.h \
    widgets/MainShowDataWidget.h \
    widgets/HomeWidget.h \
    widgets/LeftSidebarWidget.h \
    toolbar.h \
    data/DataManager.h \
    widgets/FileAttrWidget.h \
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
    listWidget/NotesItemWidget.h

DISTFILES += \
    themes/solarized_light.theme \
    themes/atom_dark.theme \
    themes/deepin_dark.theme \
    themes/solarized_dark.theme \
    themes/deepin.theme

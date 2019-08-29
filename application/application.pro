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
    widgets/BookMarkWidget.cpp \
    widgets/NotesWidget.cpp \
    widgets/ThumbnailWidget.cpp \
    widgets/PagingWidget.cpp


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
    widgets/BookMarkWidget.h \
    widgets/NotesWidget.h \
    widgets/ThumbnailWidget.h \
    widgets/PagingWidget.h

DISTFILES += \
    themes/solarized_light.theme \
    themes/atom_dark.theme \
    themes/deepin_dark.theme \
    themes/solarized_dark.theme \
    themes/deepin.theme

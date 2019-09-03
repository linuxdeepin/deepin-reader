QT += core gui
QT += dtkwidget
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin_reader
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += poppler-qt5

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
    widgets/MainShowSplitter.cpp \
    data/DataManager.cpp \
    listWidget/PagingWidget.cpp \
    listWidget/NotesWidget.cpp \
    listWidget/BookMarkWidget.cpp \
    listWidget/ThumbnailItemWidget.cpp \
    listWidget/ThumbnailWidget.cpp \
    listWidget/BookMarkItemWidget.cpp \
    listWidget/NotesItemWidget.cpp \
    header/MsgSubject.cpp \
    header/CustomWidget.cpp\
    pdfview/documentview.cpp \
    pdfview/pdfmodel.cpp \
    pdfview/loadhandler.cpp \
    view/MagnifyingWidget.cpp \
    view/DefaultOperationWidget.cpp \
    view/TextOperationWidget.cpp \
    view/LightedWidget.cpp \
    view/FileViewWidget.cpp \
    view/FileAttrWidget.cpp \
    widgets/FindBar.cpp \
    widgets/LineBar.cpp \
    listWidget/MyLabel.cpp



RESOURCES +=         resources.qrc

HEADERS +=\
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
    widgets/MainShowSplitter.h \
    data/DataManager.h \
    header/ISubject.h \
    header/IObserver.h \
    header/MsgSubject.h \
    header/MsgHeader.h \
    header/CustomWidget.h\
    listWidget/ThumbnailItemWidget.h \
    listWidget/NotesWidget.h \
    listWidget/ThumbnailWidget.h \
    listWidget/PagingWidget.h \
    listWidget/BookMarkWidget.h \
    listWidget/BookMarkItemWidget.h \
    listWidget/NotesItemWidget.h \   
    pdfview/documentview.h \
    pdfview/model.h \
    pdfview/global.h \
    pdfview/pdfmodel.h \
    pdfview/loadhandler.h \
    view/MagnifyingWidget.h \
    view/DefaultOperationWidget.h \
    view/TextOperationWidget.h \
    view/LightedWidget.h \
    view/FileViewWidget.h \
    view/FileAttrWidget.h \
    widgets/FindBar.h \
    widgets/LineBar.h \
    listWidget/MyLabel.h

DISTFILES += \
    themes/solarized_light.theme \
    themes/atom_dark.theme \
    themes/deepin_dark.theme \
    themes/solarized_dark.theme \
    themes/deepin.theme\



QT += core gui svg
QT += dtkwidget
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = deepin_reader
TEMPLATE = app
CONFIG += c++11 link_pkgconfig
PKGCONFIG += x11 poppler-qt5

include (frame/frame.pri)
include (utils/utils.pri)
include (controller/controller.pri)
include (pdfControl/pdfControl.pri)
include (docview/docview.pri)
include (subjectObserver/subjectObserver.pri)
include (mainShow/mainShow.pri)

SOURCES += \
        main.cpp \
    application.cpp
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
    view/MagnifyingWidget.cpp \
    view/DefaultOperationWidget.cpp \
    view/TextOperationWidget.cpp \
    view/LightedWidget.cpp \
    view/FileViewWidget.cpp \
    view/FileAttrWidget.cpp \
    widgets/FindBar.cpp \
    widgets/LineBar.cpp \
    listWidget/MyLabel.cpp \
    docview/docummentproxy.cpp \
    docview/docummentbase.cpp \
    docview/docummentfactory.cpp \
    docview/pdf/docummentpdf.cpp \
    docview/pagebase.cpp \
    docview/pdf/pagepdf.cpp \
    docview/publicfunc.cpp
#\
#    docview/pdf/pdfmodel.cpp \
#    docview/documentlayout.cpp \
#    docview/documentview.cpp \
#    docview/loadhandler.cpp \
#    docview/pageitem.cpp \
#    docview/rendertask.cpp \
#    docview/tileitem.cpp \
#    docview/viewsettings.cpp



RESOURCES +=         resources.qrc

HEADERS +=\
    application.h
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
    view/MagnifyingWidget.h \
    view/DefaultOperationWidget.h \
    view/TextOperationWidget.h \
    view/LightedWidget.h \
    view/FileViewWidget.h \
    view/FileAttrWidget.h \
    widgets/FindBar.h \
    widgets/LineBar.h \
    listWidget/MyLabel.h \
    docview/docummentproxy.h \
    docview/docummentbase.h \
    docview/docummentfactory.h \
    docview/pdf/docummentpdf.h \
    docview/pagebase.h \
    docview/pdf/pagepdf.h \
    docview/publicfunc.h
#\
#    docview/pdf/pdfmodel.h \
#    docview/documentlayout.h \
#    docview/documentview.h \
#    docview/global.h \
#    docview/loadhandler.h \
#    docview/model.h \
#    docview/pageitem.h \
#    docview/renderparam.h \
#    docview/rendertask.h \
#    docview/tileitem.h \
#    docview/viewsettings.h

DISTFILES += \
    themes/solarized_light.theme \
    themes/atom_dark.theme \
    themes/deepin_dark.theme \
    themes/solarized_dark.theme \
    themes/deepin.theme

APPICONDIR = $$PREFIX/share/icons/deepin/apps/scalable

app_icon.path = $$APPICONDIR
app_icon.files = $$PWD/resources/image/logo/logo.svg

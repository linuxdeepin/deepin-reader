INCLUDEPATH += $$PWD
INCLUDEPATH += $$PWD/uiframe
INCLUDEPATH += $$PWD/widgets
INCLUDEPATH += $$PWD/sidebar
INCLUDEPATH += $$PWD/browser
INCLUDEPATH += $$PWD/document

include ($$PWD/app/app.pri)
include ($$PWD/browser/browser.pri)
include ($$PWD/sidebar/sidebar.pri)
include ($$PWD/widgets/widgets.pri)
include ($$PWD/document/document.pri)

SOURCES += \
    $$PWD/Application.cpp \
    $$PWD/MainWindow.cpp \
    $$PWD/uiframe/TitleMenu.cpp \
    $$PWD/uiframe/TitleWidget.cpp \
    $$PWD/uiframe/Central.cpp \
    $$PWD/uiframe/CentralNavPage.cpp \
    $$PWD/uiframe/CentralDocPage.cpp \
    $$PWD/uiframe/DocTabBar.cpp \
    $$PWD/uiframe/DocSheet.cpp \
    $$PWD/uiframe/SheetRenderer.cpp

HEADERS +=\
    $$PWD/Application.h\
    $$PWD/MainWindow.h \
    $$PWD/uiframe/TitleWidget.h \
    $$PWD/uiframe/TitleMenu.h \
    $$PWD/uiframe/Central.h \
    $$PWD/uiframe/CentralNavPage.h \
    $$PWD/uiframe/CentralDocPage.h \
    $$PWD/uiframe/DocTabBar.h \
    $$PWD/uiframe/DocSheet.h \
    $$PWD/uiframe/SheetRenderer.h

RESOURCES    += $$PWD/../resources/resources.qrc

#link
#由于自动化构建暂时无法自动下载sub module,目前手动内置第三方库
INCLUDEPATH += $$PWD/../3rdparty/deepin-pdfium/include
LIBS += -L$$PWD/../3rdparty/deepin-pdfium/lib -ldeepin-pdfium

#ifndef WIDGETHEADER_H
#define WIDGETHEADER_H

#include <QMap>
#include <QString>

namespace  DR_SPACE {

enum E_WIDGET {
    E_MAIN_WINDOW,
    E_TITLE_WIDGET,
    E_PLAY_CONTROL_WIDGET,
    E_NOTE_TIP_WIDGET,
    E_MAIN_OPERATION_WIDGET,
    E_LEFT_SLIDERBAR_WIDGET,
    E_HOME_WIDGET,
    E_FIND_WIDGET,
    E_FILE_VIEW_WIDGET,
    E_FILE_ATTR_WIDGET,
    E_DOC_SHOW_SHELL_WIDGET,
    E_CENTRAL_WIDGET,
    E_MAIN_TAB_WIDGET,
    E_MAIN_SPLITTER,
    E_THUMBAIL_WIDGET,
    E_THUMBAIL_ITEM_WIDGET,
    E_PAGE_WIDGET,
    E_SEARCH_RES_WIDGET,
    E_SEARCH_ITEM_WIDGET,
    E_SPINNER_WIDGET,
    E_NOTE_VIEW_WIDGET,
    E_NOTE_WIDGET,
    E_NOTE_ITEM_WIDGET,
    E_CATALOG_WIDGET,
    E_BOOKMARK_WIDGET,
    E_BOOKMARK_ITEM_WIDGET
};

static QString MAIN_WINDOW = "MainWindow";
static QString TITLE_WIDGET = "TitleWidget";
static QString PLAY_CONTROL_WIDGET = "PlayControlWidget";
static QString NOTE_TIP_WIDGET = "NoteTipWidget";
static QString MAIN_OPERATION_WIDGET = "MainOperationWidget";
static QString LEFT_SLIDERBAR_WIDGET = "SheetSidebar";
static QString HOME_WIDGET = "HomeWidget";
static QString FIND_WIDGET = "FindWidget";
static QString FILE_VIEW_WIDGET = "FileViewWidget";
static QString FILE_ATTR_WIDGET = "FileAttrWidget";
static QString DOC_SHOW_SHELL_WIDGET = "DocShowShellWidget";
static QString CENTRAL_WIDGET = "CentralWidget";
static QString MAIN_TAB_WIDGET = "CentralDocPage";
static QString MAIN_TAB_BAR = "DocTabBar";
static QString MAIN_SPLITTER = "DocSheet";
static QString THUMBAIL_WIDGET = "ThumbnailWidget";
static QString THUMBAIL_ITEM_WIDGET = "ThumbnailItemWidget";
static QString PAGE_WIDGET = "PagingWidget";
static QString SEARCH_RES_WIDGET = "SearchResWidget";
static QString SEARCH_ITEM_WIDGET = "SearchItemWidget";
static QString SPINNER_WIDGET = "BufferWidget";
static QString NOTE_VIEW_WIDGET = "NoteViewWidget";
static QString NOTE_WIDGET = "NoteWidget";
static QString NOTE_ITEM_WIDGET = "NoteItemWidget";
static QString CATALOG_WIDGET = "CatalogWidget";
static QString BOOKMARK_WIDGET = "BookMarkWidget";
static QString BOOKMARK_ITEM_WIDGET = "BookMarkItemWidget";


enum E_MENU {
    E_TITLE_MENU,
    E_TEXT_OPERATION_MENU,
    E_SCALE_MENU,
    E_HANDLE_MENU,
    E_FONT_MENU,
    E_DEFAULT_OPERATION_MENU,
    E_NOTE_MENU,
    E_BOOKMARK_MENU
};

static QString TITLE_MENU = "TitleMenu";
static QString TEXT_OPERATION_MENU = "TextOperationMenu";
static QString SCALE_MENU = "ScaleMenu";
static QString HANDLE_MENU = "HandleMenu";
static QString FONT_MENU = "FontMenu";
static QString DEFAULT_OPERATION_MENU = "DefaultOperationMenu";
static QString NOTE_MENU = "NOTE_MENU";
static QString BOOKMARK_MENU = "BOOKMARK_MENU";


}

#endif // WIDGETHEADER_H

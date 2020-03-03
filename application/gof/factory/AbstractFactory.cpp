#include "AbstractFactory.h"

#include "WidgetHeader.h"

using namespace DR_SPACE;

#include "menu/DefaultOperationMenu.h"
#include "menu/FontMenu.h"
#include "menu/HandleMenu.h"
#include "menu/ScaleMenu.h"
#include "menu/TextOperationMenu.h"
#include "menu/TitleMenu.h"

#include "widgets/CentralWidget.h"
#include "widgets/DocShowShellWidget.h"
#include "widgets/main/MainTabWidgetEx.h"

AbstractFactory::AbstractFactory()
{

}

AbstractFactory::~AbstractFactory()
{

}

ConcreteFactory::ConcreteFactory()
{
}

ConcreteFactory::~ConcreteFactory()
{

}

CustomWidget *ConcreteFactory::CreateWidget(const int &iType, DWidget *parent)
{
    CustomWidget *pWidget = nullptr;

    if (parent) {
        CustomWidget *parentWidget = qobject_cast<CustomWidget *>(parent);

        switch (iType) {
        case E_MAIN_WINDOW :
        case E_TITLE_WIDGET:
        case E_PLAY_CONTROL_WIDGET:
        case E_NOTE_TIP_WIDGET:
        case E_MAIN_OPERATION_WIDGET:
        case E_LEFT_SLIDERBAR_WIDGET:
        case E_HOME_WIDGET:
        case E_FIND_WIDGET:
        case E_FILE_VIEW_WIDGET:
        case E_FILE_ATTR_WIDGET:
        case E_DOC_SHOW_SHELL_WIDGET:
            pWidget = new DocShowShellWidget(parentWidget);
            break;
        case E_CENTRAL_WIDGET:
            pWidget = new CentralWidget(parentWidget);
            break;
        case E_MAIN_TAB_WIDGET:
            pWidget = new MainTabWidgetEx(parentWidget);
            break;
        default:
            break;
        }
    }
    return  pWidget;
}

CustomMenu *ConcreteFactory::CreateMenu(const int &iType, DWidget *parent)
{
    CustomMenu *pMenu = nullptr;
    switch (iType) {
    case E_TITLE_MENU:
        pMenu = new TitleMenu(parent);
        break;
    case E_TEXT_OPERATION_MENU:
        pMenu = new TextOperationMenu(parent);
        break;
    case E_SCALE_MENU:
        pMenu = new ScaleMenu(parent);
        break;
    case E_HANDLE_MENU:
        pMenu = new HandleMenu(parent);
        break;
    case E_FONT_MENU:
        pMenu = new FontMenu(parent);
        break;
    case E_DEFAULT_OPERATION_MENU:
        pMenu = new DefaultOperationMenu(parent);
        break;
    default:
        break;
    }
    return pMenu;
}

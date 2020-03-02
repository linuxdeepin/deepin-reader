#include "AbstractFactory.h"

#include "WidgetHeader.h"

using namespace DR_SPACE;

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

CustomWidget *ConcreteFactory::CreateWidget(const int& iType, DWidget * parent)
{
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
   case E_CENTRAL_WIDGET:
   case E_MAIN_TAB_WIDGET:
        break;
    default:
        break;
    }
    return  nullptr;
}

CustomMenu*ConcreteFactory::CreateMenu(const int& iType, DWidget * parent)
{
    return nullptr;
}

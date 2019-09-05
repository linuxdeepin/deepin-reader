#include "frame/MainWindow.h"
#include "application.h"
#include <DLog>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    // Init DTK.
    Application::loadDXcbPlugin();

    Application a(argc, argv);
    a.setAttribute(Qt::AA_UseHighDpiPixmaps);
    a.setAttribute(Qt::AA_EnableHighDpiScaling);
//    a.setAttribute(Qt::AA_ForceRastermainShow);

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    MainWindow w;
    w.setMinimumSize(500, 500);
    w.show();

    return a.exec();
}

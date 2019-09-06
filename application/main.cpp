#include "frame/MainWindow.h"
#include "application.h"
#include <DLog>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    // Init DTK.
    Application::loadDXcbPlugin();

    Application a(argc, argv);

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    MainWindow w;
    w.show();

    return a.exec();
}

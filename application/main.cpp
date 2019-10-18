#include "frame/MainWindow.h"
#include "application.h"
#include <DLog>
#include <QLabel>
#include <QApplication>

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
//    QApplication a(argc,argv);
//    QMainWindow w;
//    QLabel* plabel=new QLabel;
//    plabel->setPixmap(QPixmap(":/resources/image/logo/deepin-reader.svg"));
//    w.setCentralWidget(plabel);
//    w.setWindowIcon(QIcon(":/resources/image/logo/deepin-reader.svg"));
//    w.resize(500,500);
//    w.show();

    return a.exec();

}

#include "MainWindow.h"
#include "application.h"
#include <DLog>
#include <DApplicationSettings>
#include <QDesktopWidget>
#include "app/ProcessController.h"
#include "app/json.h"
#include "app/FileController.h"
#include <QLockFile>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    // Init DTK.
    Application::loadDXcbPlugin();

    Application a(argc, argv);

    QCommandLineParser parser;

    parser.process(a);

    QStringList arguments = parser.positionalArguments();

    //进程同步 解决选中多个文件同时右击打开不在同一个窗口的问题
    QLockFile file("./.deepin-reader-lock");
    int tryTimes = 0;
    while (!file.tryLock(1000)) {
        QThread::msleep(100);
        if (tryTimes++ > 4)
            break;
    }

    //=======通知已经打开的进程
    ProcessController controller;

    if (controller.openIfAppExist(arguments)) {
        file.unlock();
        return 0;
    }
    //通知==========END

    DApplicationSettings savetheme;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    QApplication::desktop()->geometry();
    a.setSreenRect(a.desktop()->geometry());

    MainWindow *w = MainWindow::create();

    if (!controller.listen())
        return -1;

    foreach (const QString &filePath, arguments) {
        w->addFile(filePath);
    }

    w->show();

    file.unlock();

    return a.exec();
}

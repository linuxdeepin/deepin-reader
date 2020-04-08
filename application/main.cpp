#include "MainWindow.h"
#include "application.h"
#include <DLog>
#include <DApplicationSettings>
#include <QDesktopWidget>
#include "app/ProcessController.h"
#include "app/json.h"
#include "app/FileController.h"

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
    QSharedMemory share;
    bool hasWaited = false;
    share.setKey("deepin_reader");
    while (!share.create(1)) {
        hasWaited = true;
        QThread::msleep(100);
    }
    if (hasWaited) {
        share.detach();
    }

    //=======通知已经打开的进程
    ProcessController controller;

    QStringList waitOpenFilePathList;

    if (arguments.count() > 0 && !arguments.contains("newwindow")) {

        foreach (const QString &path, arguments) {

            if (FileController::FileType_UNKNOWN != FileController::getFileType(path)) {
                QString filePath = FileController::getUrlInfo(path).toLocalFile();
                if (!controller.existFilePath(filePath))    //存在则直接通知 本程序不打开
                    waitOpenFilePathList.append(filePath);
            }
        }

        if (waitOpenFilePathList.isEmpty()) {
            share.detach();
            return 0;
        }

        if (controller.openIfAppExist(waitOpenFilePathList)) {
            share.detach();
            return 0;
        }

        if (hasWaited) {
            QThread::msleep(500);      //经测试 存在一个 其他的等待10秒后 依旧打开新窗口
        }

        if (controller.openIfAppExist(waitOpenFilePathList)) {
            share.detach();
            return 0;
        }
    } else {
        waitOpenFilePathList = arguments;
        waitOpenFilePathList.removeOne("newwindow");
    }
    //通知==========END

    DApplicationSettings savetheme;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    QApplication::desktop()->geometry();
    a.setSreenRect(a.desktop()->geometry());

    MainWindow *w = MainWindow::create();

    controller.listen();

    share.detach();

    foreach (const QString &filePath, waitOpenFilePathList) {
        w->addFile(filePath);
    }

    w->show();

    return a.exec();
}

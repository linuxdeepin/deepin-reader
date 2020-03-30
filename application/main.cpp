#include "MainWindow.h"
#include "application.h"
#include <DLog>
#include <DApplicationSettings>
#include <QDesktopWidget>
#include "app/processcontroller.h"
#include "app/json.h"

DWIDGET_USE_NAMESPACE

QUrl UrlInfo(QString path)
{
    QUrl url;
    // Just check if the path is an existing file.
    if (QFile::exists(path)) {
        url = QUrl::fromLocalFile(QDir::current().absoluteFilePath(path));
        return url;
    }

    const auto match = QRegularExpression(QStringLiteral(":(\\d+)(?::(\\d+))?:?$")).match(path);

    if (match.isValid()) {
        // cut away line/column specification from the path.
        path.chop(match.capturedLength());
    }

    // make relative paths absolute using the current working directory
    // prefer local file, if in doubt!
    url = QUrl::fromUserInput(path, QDir::currentPath(), QUrl::AssumeLocalFile);

    // in some cases, this will fail, e.g.
    // assume a local file and just convert it to an url.
    if (!url.isValid()) {
        // create absolute file path, we will e.g. pass this over dbus to other processes
        url = QUrl::fromLocalFile(QDir::current().absoluteFilePath(path));
    }
    return url;
}

int main(int argc, char *argv[])
{
    // Init DTK.
    Application::loadDXcbPlugin();

    Application a(argc, argv);

    QCommandLineParser parser;

    parser.process(a);

    QStringList arguments = parser.positionalArguments();

    //进程同步
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

            QString filePath = UrlInfo(path).toLocalFile();

            if (filePath.endsWith("pdf")) {

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
    //通知==========

    DApplicationSettings savetheme;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    QApplication::desktop()->geometry();
    a.setSreenRect(a.desktop()->geometry());

    MainWindow w;

    if (arguments.contains("newwindow"))
        w.move(QCursor::pos());

    controller.listen();

    foreach (const QString &filePath, waitOpenFilePathList) {
        if (filePath.endsWith("pdf")) {
            w.openfile(filePath);
        }
    }

    w.show();

    share.detach();

    return a.exec();
}

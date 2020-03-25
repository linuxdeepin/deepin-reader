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
    DApplicationSettings savetheme;

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    QCommandLineParser parser;
    parser.process(a);
    QStringList arguments = parser.positionalArguments();
    ProcessController controller;
    QStringList filePathList = arguments;

    QApplication::desktop()->geometry();
    a.setSreenRect(a.desktop()->geometry());

    MainWindow w;

    if (!arguments.isEmpty()) {

        if (arguments.contains("newwindow")) {
            foreach (const QString &filePath, filePathList) {
                if (filePath.endsWith("pdf")) {
                    w.openfile(filePath);
                }
            }
            w.move(QCursor::pos());
        } else {
            QStringList waitOpenFilePathList;

            foreach (const QString &path, arguments) {

                QString filePath = UrlInfo(path).toLocalFile();

                if (filePath.endsWith("pdf")) {

                    if (!controller.existFilePath(filePath))    //存在则直接通知 本程序不打开
                        waitOpenFilePathList.append(filePath);
                }
            }

            if (waitOpenFilePathList.isEmpty())
                return 0;

            if (controller.openIfAppExist(waitOpenFilePathList))
                return 0;

            foreach (const QString &filePath, waitOpenFilePathList)
                w.openfile(filePath);
        }
    }

//    QApplication::desktop()->geometry();
//    w.setSreenRect(a.desktop()->screenGeometry());
    w.show();

    controller.listen();

    return a.exec();
}

#include "MainWindow.h"
#include "application.h"
#include <DLog>
#include <DApplicationSettings>
#include <QDesktopWidget>

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
    MainWindow w;
    QCommandLineParser parser;
    parser.process(a);

    QStringList urls;
    QStringList arguments = parser.positionalArguments();

    QString filepath;
    foreach (const QString &path, arguments) {
        filepath = UrlInfo(path).toLocalFile();
        if (filepath.endsWith("pdf")) {
            w.openfile(filepath);
            qDebug() << __FUNCTION__ << "++++++++++++++" << path << arguments;
            break;
        } else {
            exit(0);
        }
    }

    QApplication::desktop()->geometry();
    w.setSreenRect(a.desktop()->screenGeometry());
    w.show();

    return a.exec();
}

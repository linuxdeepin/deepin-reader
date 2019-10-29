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
    QCommandLineParser parser;
    parser.process(a);

    QStringList urls;
    QStringList arguments = parser.positionalArguments();
    QString filepath;
    for (const QString &path : arguments) {
        if (QFile::exists(path))
            continue;
        filepath=QUrl::fromLocalFile(QDir::current().absoluteFilePath(path)).toLocalFile();
        if(filepath.endsWith("pdf"))
        {
            w.openfile(filepath);
            break;
        }
    }

    w.show();

    return a.exec();

}



#include <DApplication>
#include <DMainWindow>
#include <DWidgetUtil>
#include <DLog>

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <iostream>
#include "window.h"

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    // Init DTK.
    DApplication::loadDXcbPlugin();
    const char *descriptionText = QT_TRANSLATE_NOOP("MainWindow",
                                                    "Deepin Editor is a desktop text editor that supports common text editing features.");
    const QString acknowledgementLink = "https://www.deepin.org/original/deepin-editor/";

    DApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps);
    app.loadTranslator();
    app.setTheme("light");
    app.setOrganizationName("deepin");
    app.setApplicationName("deepin_reader");
    app.setApplicationDisplayName(QObject::tr("Deepin Reader"));
    app.setApplicationVersion("1.0");
    app.setProductIcon(QIcon(":/images/logo.svg"));
    app.setProductName(DApplication::translate("MainWindow", "Deepin Reader"));
    app.setApplicationDescription(DApplication::translate("MainWindow", descriptionText) + "\n");
    app.setApplicationAcknowledgementPage(acknowledgementLink);

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    // Parser input arguments.
    QCommandLineParser parser;
    const QCommandLineOption newWindowOption("w", "Open file in new window");
    const QCommandLineOption helpOption = parser.addHelpOption();
    parser.addOption(newWindowOption);
    parser.process(app);

    QStringList urls;
    QStringList arguments = parser.positionalArguments();

    for (const QString &path : arguments) {
        //UrlInfo info(path);
        //urls << info.url.toLocalFile();
    }

    bool hasWindowFlag = parser.isSet(newWindowOption);

    Window w;
    w.setMinimumSize(500, 500);
    w.show();

    Dtk::Widget::moveToCenter(&w);

    return app.exec();
}

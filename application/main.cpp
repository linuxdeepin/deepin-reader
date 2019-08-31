#include "window.h"
#include <DApplication>
#include <DLog>

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

    Window w;
    w.setMinimumSize(500, 500);
    w.show();

    return app.exec();
}

#include "MainWindow.h"
#include "application.h"
#include <DLog>
#include <DApplicationSettings>
#include <QDesktopWidget>
#include "app/ProcessController.h"
#include "app/json.h"
#include <QLockFile>
#include <QDebug>
#include <QDBusConnection>
#include <QDBusInterface>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    // Init DTK.
    Application::loadDXcbPlugin();

    Application a(argc, argv);

    QCommandLineParser parser;

    parser.process(a);

    QStringList arguments = parser.positionalArguments();

    //=======通知已经打开的进程
    QDBusConnection dbus = QDBusConnection::sessionBus();
    if (!dbus.registerService("com.deepin.Reader")) {
        QDBusInterface notification("com.deepin.Reader", "/com/deepin/Reader", "com.deepin.Reader", QDBusConnection::sessionBus());
        QList<QVariant> args;
        args.append(arguments);
        notification.callWithArgumentList(QDBus::Block, "handleFiles", args).errorMessage();
        return 0;
    }

    ProcessController controller;
    dbus.registerObject("/com/deepin/Reader", &controller, QDBusConnection::ExportScriptableSlots);

    DApplicationSettings savetheme;
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();
    QApplication::desktop()->geometry();
    a.setSreenRect(a.desktop()->geometry());

    MainWindow *w = MainWindow::createWindow();

    foreach (const QString &filePath, arguments) {
        w->doOpenFile(filePath);
    }

    w->show();

    return a.exec();
}

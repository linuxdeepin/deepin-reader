#include "MainWindow.h"
#include "Application.h"
#include "CentralDocPage.h"

#include <DLog>
#include <QDebug>
#include <QDesktopWidget>
#include <QDBusConnection>
#include <QDBusInterface>
#include <DApplicationSettings>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    // Init DTK.
    Application a(argc, argv);

    QCommandLineParser parser;

    parser.addOptions({
        {
            {"f", "filePath"},
            QCoreApplication::translate("main", "Document File Path."),
            QCoreApplication::translate("main", "FilePath")
        },
        {   {"t", "thumbnailPath"},
            QCoreApplication::translate("main", "ThumbnailPath Path."),
            QCoreApplication::translate("main", "FilePath")
        },
        {
            "thumbnail",
            QCoreApplication::translate("main", "Generate thumbnail.")
        }
    });

    parser.process(a);

    if (parser.isSet("thumbnail") && parser.isSet("filePath") && parser.isSet("thumbnailPath")) {
        QString filePath = parser.value("filePath");
        QString thumbnailPath = parser.value("thumbnailPath");
        if (filePath.isEmpty() || thumbnailPath.isEmpty())
            return -1;

        if (!CentralDocPage::firstThumbnail(filePath, thumbnailPath))
            return -1;

        return 0;
    }

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

    dbus.registerObject("/com/deepin/Reader", &a, QDBusConnection::ExportScriptableSlots);

    DApplicationSettings savetheme;
    Q_UNUSED(savetheme)

    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    if (!MainWindow::allowCreateWindow())
        return -1;

    MainWindow *w = MainWindow::createWindow();

    foreach (const QString &filePath, arguments) {
        w->doOpenFile(filePath);
    }

    w->show();

    return a.exec();
}

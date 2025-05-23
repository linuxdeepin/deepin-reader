// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MainWindow.h"
#include "Application.h"
#include "CentralDocPage.h"
#include "accessible.h"
#include "Utils.h"
#include "DBusObject.h"

#include <DLog>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <DApplicationSettings>
#endif

#include <QCommandLineParser>
#include <QScreen>
#include <QAccessible>
#include <QDebug>
#include <QFontDatabase>

DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    PERF_PRINT_BEGIN("POINT-01", "");
    qDebug() << "Application starting with arguments:" << argc;
    qDebug() << "Command line:" << QCoreApplication::arguments().join(" ");

    // 依赖DTK的程序，如果要在root下或者非deepin/uos环境下运行不会发生异常，就需要加上该环境变量
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1); //setenv改变或添加一个环境变量
    }

    // Init DTK.
    qDebug() << "Initializing DTK application";
    Application a(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
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
        qDebug() << "Generating thumbnail for:" << filePath;
        qDebug() << "Thumbnail output path:" << thumbnailPath;
        if (filePath.isEmpty() || thumbnailPath.isEmpty()) {
            qWarning() << "Empty file path or thumbnail path";
            return -1;
        }

        if (!CentralDocPage::firstThumbnail(filePath, thumbnailPath)) {
            qWarning() << "Failed to generate thumbnail";
            return -1;
        }

        return 0;
    }

    QStringList arguments = parser.positionalArguments();

    if (arguments.size() > 0)
        PERF_PRINT_BEGIN("POINT-05", "");

    //=======通知已经打开的进程
    qDebug() << "Registering DBus service";
    if (!DBusObject::instance()->registerOrNotify(arguments)) {
        qInfo() << "Another instance is running, exiting";
        return 0;
    }

    QAccessible::installFactory(accessibleFactory);

    //Dtk自动保存主题
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    DApplicationSettings savetheme;
    Q_UNUSED(savetheme)
#endif

    qDebug() << "Setting up log appenders";
    Dtk::Core::DLogManager::registerConsoleAppender();
    Dtk::Core::DLogManager::registerFileAppender();

    qDebug() << "Checking if new window can be created";
    if (!MainWindow::allowCreateWindow()) {
        qWarning() << "Maximum window count reached";
        return -1;
    }

    qDebug() << __FUNCTION__ << "正在创建主窗口...";
    qApp->setAttribute(Qt::AA_ForceRasterWidgets, true);
    MainWindow *w = MainWindow::createWindow(arguments);
    w->winId();
    qApp->setAttribute(Qt::AA_ForceRasterWidgets, false);
    w->show();

    qDebug() << __FUNCTION__ << "主窗口已创建并显示";

    PERF_PRINT_END("POINT-01", "");

    qDebug() << "Entering main event loop";
    int result = a.exec();

    PERF_PRINT_END("POINT-02", "");
    qInfo() << "Application exiting with code:" << result;

    return result;
}

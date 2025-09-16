// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MainWindow.h"
#include "Application.h"
#include "CentralDocPage.h"
#include "accessible.h"
#include "Utils.h"
#include "DBusObject.h"
#include "ddlog.h"
#include "logger.h"

#include <DLog>

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <DApplicationSettings>
#endif

#include <QCommandLineParser>
#include <QScreen>
#include <QAccessible>
#include <QDebug>
#include <QFontDatabase>

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

int main(int argc, char *argv[])
{
    // set log format and register console and file appenders
    const QString logFormat = "%{time}{yy-MM-ddTHH:mm:ss.zzz} [%{type:-7}] [%{category}] <%{function}:%{line}> %{message}";
    DLogManager::setLogFormat(logFormat);
    // 为了兼容性
#if (DTK_VERSION >= DTK_VERSION_CHECK(5, 6, 8, 0))
    DLogManager::registerJournalAppender();
#endif
    DLogManager::registerConsoleAppender();

    MLogger();   // 日志处理要放在app之前，否则QApplication内部可能进行了日志打印，导致环境变量设置不生效

    PERF_PRINT_BEGIN("POINT-01", "");
    qCDebug(appLog) << "Application starting with arguments:" << argc;
    qCDebug(appLog) << "Command line:" << QCoreApplication::arguments().join(" ");

    // 依赖DTK的程序，如果要在root下或者非deepin/uos环境下运行不会发生异常，就需要加上该环境变量
    if (!QString(qgetenv("XDG_CURRENT_DESKTOP")).toLower().startsWith("deepin")) {
        setenv("XDG_CURRENT_DESKTOP", "Deepin", 1); //setenv改变或添加一个环境变量
    }

    // Init DTK.
    qCDebug(appLog) << "Initializing DTK application";
    Application a(argc, argv);

    // register file appender must after Application instance created
    DLogManager::registerFileAppender();

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
        qCDebug(appLog) << "Generating thumbnail for:" << filePath;
        qCDebug(appLog) << "Thumbnail output path:" << thumbnailPath;
        if (filePath.isEmpty() || thumbnailPath.isEmpty()) {
            qCWarning(appLog) << "Empty file path or thumbnail path";
            return -1;
        }

        if (!CentralDocPage::firstThumbnail(filePath, thumbnailPath)) {
            qCWarning(appLog) << "Failed to generate thumbnail";
            return -1;
        }

        return 0;
    }

    QStringList arguments = parser.positionalArguments();

    if (arguments.size() > 0)
        PERF_PRINT_BEGIN("POINT-05", "");

    //=======通知已经打开的进程
    qCDebug(appLog) << "Registering DBus service";
    if (!DBusObject::instance()->registerOrNotify(arguments)) {
        qCInfo(appLog) << "Another instance is running, exiting";
        return 0;
    }

    QAccessible::installFactory(accessibleFactory);

    //Dtk自动保存主题
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    DApplicationSettings savetheme;
    Q_UNUSED(savetheme)
#endif

    qCDebug(appLog) << "Checking if new window can be created";
    if (!MainWindow::allowCreateWindow()) {
        qCWarning(appLog) << "Maximum window count reached";
        return -1;
    }

    qCDebug(appLog) << __FUNCTION__ << "正在创建主窗口...";
    qApp->setAttribute(Qt::AA_ForceRasterWidgets, true);
    MainWindow *w = MainWindow::createWindow(arguments);
    w->winId();
    qApp->setAttribute(Qt::AA_ForceRasterWidgets, false);
    w->show();

    qCDebug(appLog) << __FUNCTION__ << "主窗口已创建并显示";

    PERF_PRINT_END("POINT-01", "");

    qCDebug(appLog) << "Entering main event loop";
    int result = a.exec();

    PERF_PRINT_END("POINT-02", "");
    qCInfo(appLog) << "Application exiting with code:" << result;

    return result;
}

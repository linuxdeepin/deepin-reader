// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "MainWindow.h"
#include "Application.h"
#include "CentralDocPage.h"
#include "accessible.h"
#include "Utils.h"
#include "DBusObject.h"
#include "Database.h"
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
#include <QFileInfo>
#include <QFontDatabase>
#include <QSet>

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

    // 启动时清理失效的状态记录
    Database::instance()->cleanupOrphanStates();

    // 这是第一个实例（没有其他 deepin-reader 在运行），恢复上次的标签页组
    // 不管命令行是否指定了文件，都恢复之前的标签页
    // 注意：当前仅恢复 windowIndex=0 的标签页组（多窗口场景的完整恢复待后续优化）
    QString initialActiveFile;
    {
        int savedActiveIndex = 0;
        QStringList restoredFiles = Database::instance()->readTabGroup(0, savedActiveIndex);
        // 过滤掉不存在或不可读的文件
        QStringList validRestored;
        for (const QString &filePath : restoredFiles) {
            QFileInfo fi(filePath);
            if (fi.exists() && fi.isReadable())
                validRestored.append(filePath);
        }

        if (!validRestored.isEmpty()) {
            // 将命令行参数中的 URL 转为本地路径
            QStringList localArguments;
            for (const QString &arg : arguments) {
                QUrl url(arg);
                if (url.isLocalFile())
                    localArguments.append(url.toLocalFile());
                else
                    localArguments.append(arg);
            }

            // 合并文件列表：保留历史标签页及其拖拽后的顺序，仅追加新打开的文件
            // 这样重新打开相同文件时能保持上次的标签顺序；打开新文件时历史不会丢失
            QStringList allFiles = validRestored;
            QSet<QString> restoredSet(validRestored.begin(), validRestored.end());
            int newFileCount = 0;
            for (const QString &fp : localArguments) {
                if (QFile::exists(fp) && !restoredSet.contains(fp)) {
                    allFiles.append(fp);
                    newFileCount++;
                }
            }
            arguments = allFiles;

            // 计算需要激活的标签页：
            // - 用户指定了文件时，激活用户指定的第一个有效文件（优先响应用户意图）
            // - 用户未指定文件时，恢复上次激活的标签页（按保存时的路径定位）
            if (!localArguments.isEmpty()) {
                for (const QString &fp : localArguments) {
                    if (QFile::exists(fp) && restoredSet.contains(fp)) {
                        initialActiveFile = fp;
                        break;
                    }
                }
                // 指定的文件不在历史中（新追加的），激活最后一个新文件
                if (initialActiveFile.isEmpty() && newFileCount > 0) {
                    initialActiveFile = allFiles.last();
                }
            } else if (savedActiveIndex >= 0 && savedActiveIndex < restoredFiles.size()) {
                QString savedActivePath = restoredFiles.at(savedActiveIndex);
                if (restoredSet.contains(savedActivePath))
                    initialActiveFile = savedActivePath;
            }

            int existingCount = allFiles.size() - newFileCount;
            qCInfo(appLog) << "Restoring" << existingCount << "tabs from last session"
                           << "+" << newFileCount << "new files"
                           << "active:" << initialActiveFile;
        }
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

    // 恢复上次激活的标签页（需在事件循环启动前设置，以覆盖 insertSheet 的延时 setCurrentIndex）
    if (!initialActiveFile.isEmpty()) {
        w->setInitialActiveFile(initialActiveFile);
    }

    w->show();

    qCDebug(appLog) << __FUNCTION__ << "主窗口已创建并显示";

    PERF_PRINT_END("POINT-01", "");

    qCDebug(appLog) << "Entering main event loop";
    int result = a.exec();

    PERF_PRINT_END("POINT-02", "");
    qCInfo(appLog) << "Application exiting with code:" << result;

    return result;
}

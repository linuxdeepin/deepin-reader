// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "DfmStandardPaths.h"
#include <QDir>
#include <QCoreApplication>
#include <QStandardPaths>
#include <QMap>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDebug>

QString DFMStandardPaths::location(DFMStandardPaths::StandardLocation type)
{
    switch (type) {
    case TrashPath:
        return QDir::homePath() + "/.local/share/Trash";
    case TrashFilesPath:
        return QDir::homePath() + "/.local/share/Trash/files";
    case TrashInfosPath:
        return QDir::homePath() + "/.local/share/Trash/info";
#ifdef APPSHAREDIR
    case TranslationPath: {
        QString path = APPSHAREDIR"/translations";
        if (!QDir(path).exists()) {
            path = qApp->applicationDirPath() + "/translations";
        }
        return path;
    }
    case TemplatesPath: {
        QString path = APPSHAREDIR"/templates";
        if (!QDir(path).exists()) {
            path = qApp->applicationDirPath() + "/templates";
        }
        return path;
    }
    case MimeTypePath: {
        QString path = APPSHAREDIR"/mimetypes";
        if (!QDir(path).exists()) {
            path = qApp->applicationDirPath() + "/mimetypes";
        }
        return path;
    }
#endif
#ifdef PLUGINDIR
    case PluginsPath: {
        QString path = PLUGINDIR;
        if (!QDir(path).exists()) {
            path = QString::fromLocal8Bit(PLUGINDIR).split(':').last();
        }
        return path;
    }
#endif
#ifdef QMAKE_TARGET
    case ApplicationConfigPath:
        return getConfigPath();
#endif
    case ThumbnailPath:
        return QDir::homePath() + "/.cache/thumbnails";
    case ThumbnailFailPath:
        return location(ThumbnailPath) + "/fail";
    case ThumbnailLargePath:
        return location(ThumbnailPath) + "/large";
    case ThumbnailNormalPath:
        return location(ThumbnailPath) + "/normal";
    case ThumbnailSmallPath:
        return location(ThumbnailPath) + "/small";
#ifdef APPSHAREDIR
    case ApplicationSharePath:
        return APPSHAREDIR;
#endif
    case RecentPath:
        return "recent:///";
    case HomePath:
        return QStandardPaths::standardLocations(QStandardPaths::HomeLocation).first();
    case DesktopPath:
        return QStandardPaths::standardLocations(QStandardPaths::DesktopLocation).first();
    case VideosPath:
        return QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).first();
    case MusicPath:
        return QStandardPaths::standardLocations(QStandardPaths::MusicLocation).first();
    case PicturesPath:
        return QStandardPaths::standardLocations(QStandardPaths::PicturesLocation).first();
    case DocumentsPath:
        return QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).first();
    case DownloadsPath:
        return QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first();
    case CachePath:
        return getCachePath();
    case DiskPath:
        return QDir::rootPath();
#ifdef NETWORK_ROOT
    case NetworkRootPath:
        return NETWORK_ROOT;
#endif
#ifdef USERSHARE_ROOT
    case UserShareRootPath:
        return USERSHARE_ROOT;
#endif
#ifdef COMPUTER_ROOT
    case ComputerRootPath:
        return COMPUTER_ROOT;
#endif
    case Root:
        return "/";
    default:
        return QStringLiteral("bug://dde-file-manager-lib/interface/dfmstandardpaths.cpp#") + QT_STRINGIFY(type);
    }
}
bool DFMStandardPaths::pathControl(const QString &sPath)
{
    QString docPath = DFMStandardPaths::location(DFMStandardPaths::DocumentsPath);
    QString picPath = DFMStandardPaths::location(DFMStandardPaths::PicturesPath);
    QDBusMessage reply;
    QDBusInterface iface("com.deepin.FileArmor1", "/com/deepin/FileArmor1", "com.deepin.FileArmor1",QDBusConnection::systemBus());
    if (iface.isValid()) {
        if(sPath.startsWith(docPath)) {
            qInfo() << "docPath";
            reply = iface.call("GetApps", docPath);
        } else if(sPath.startsWith(picPath)) {
            qInfo() << "picPath";
            reply = iface.call("GetApps", picPath);
        }
        qInfo() << "iface isValid";
    }
    if(reply.type() == QDBusMessage::ReplyMessage) {
        QList<QString> lValue = reply.arguments().takeFirst().toStringList();
        QString strApp = QStandardPaths::findExecutable("deepin-reader");
        qInfo() << "lValue :" << lValue <<" strApp: " << strApp;
        if(lValue.contains(strApp)) {
            return true;
        }
    }
    return false;
}

#ifdef QMAKE_TARGET
QString DFMStandardPaths::getConfigPath()
{
    QString projectName = QMAKE_TARGET;
    QDir::home().mkpath(".config");
    QDir::home().mkpath(QString("%1/deepin/%2/").arg(".config", projectName));
    QString defaultPath = QString("%1/%2/deepin/%3").arg(QDir::homePath(), ".config", projectName);
    return defaultPath;
}
#endif

QString DFMStandardPaths::getCachePath()
{
    QString projectName = qApp->applicationName();
    QDir::home().mkpath(".cache");
    QDir::home().mkpath(QString("%1/deepin/%2/").arg(".cache", projectName));
    QString defaultPath = QString("%1/%2/deepin/%3").arg(QDir::homePath(), ".cache", projectName);
    return defaultPath;
}

DFMStandardPaths::DFMStandardPaths() = default;

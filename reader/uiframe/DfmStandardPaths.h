// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DFMSTANDARDPATHS_H
#define DFMSTANDARDPATHS_H

#include <QString>
#include <QStandardPaths>

class DUrl;
class DFMStandardPaths
{
public:
    enum StandardLocation {
        TrashPath,
        TrashFilesPath,
        TrashInfosPath,
        TranslationPath,
        TemplatesPath,
        MimeTypePath,
        PluginsPath,
        ThumbnailPath,
        ThumbnailFailPath,
        ThumbnailLargePath,
        ThumbnailNormalPath,
        ThumbnailSmallPath,
        ApplicationConfigPath,  /* ~/.config */
        ApplicationSharePath,   /* /usr/share/dde-file-manager */
        RecentPath,
        HomePath,
        DesktopPath,
        VideosPath,
        MusicPath,
        PicturesPath,
        DocumentsPath,
        DownloadsPath,
        CachePath,              /* ~/.cache/dde-file-manager */
        DiskPath,
        NetworkRootPath,
        UserShareRootPath,
        ComputerRootPath,
        Root
    };

    static QString location(StandardLocation type);
    static bool pathControl(const QString &sPath);
    //static QString fromStandardUrl(const DUrl &standardUrl);
    //static DUrl toStandardUrl(const QString &localPath);

#ifdef QMAKE_TARGET
    static QString getConfigPath();
#endif
    static QString getCachePath();

private:
    DFMStandardPaths();
};

#endif // DFMSTANDARDPATHS_H

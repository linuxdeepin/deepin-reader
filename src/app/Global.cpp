/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "Global.h"

#include <QMimeType>
#include <QMimeDatabase>

namespace Dr {
FileType fileType(const QString &filePath)
{
    FileType fileType = FileType::Unknown;

    const QMimeType mimeType = QMimeDatabase().mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

    if (mimeType.name() == QLatin1String("application/pdf")) {
        fileType = PDF;
    } else if (mimeType.name() == QLatin1String("application/postscript")) {
        fileType = PS;
    } else if (mimeType.name() == QLatin1String("image/vnd.djvu")) {
        fileType = DjVu;
    } else if (mimeType.name() == QLatin1String("image/vnd.djvu+multipage")) {
        fileType = DjVu;
    } else if (mimeType.name() == QLatin1String("application/msword")) {
        fileType = DOC;
    } else if (mimeType.name() == QLatin1String("application/x-ole-storage")) {
        fileType = PPT_XLS;
    }

    return fileType;
}
}

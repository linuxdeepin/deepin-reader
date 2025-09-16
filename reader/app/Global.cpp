// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Global.h"
#include "ddlog.h"

#include <QMimeType>
#include <QMimeDatabase>
#include <QDebug>
#include <DGuiApplicationHelper>

namespace Dr {
FileType fileType(const QString &filePath)
{
    qCDebug(appLog) << "Detecting file type for:" << filePath;
    FileType fileType = FileType::Unknown;

    //具体的MIME文件类型可以参考https://baike.baidu.com/item/MIME/2900607?fr=aladdin
    const QMimeType mimeType = QMimeDatabase().mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

    qCInfo(appLog) << "File MIME type:" << mimeType.name();
    qCDebug(appLog) << "Matching file type...";
    if (mimeType.name() == QLatin1String("application/pdf")) {
        qCDebug(appLog) << "Matched PDF file type";
        fileType = PDF;
    } else if (mimeType.name() == QLatin1String("application/postscript")) {
        qCDebug(appLog) << "Matched PS file type";
        fileType = PS;
    } else if (mimeType.name() == QLatin1String("image/vnd.djvu") || mimeType.name() == QLatin1String("image/vnd.djvu+multipage")) {
        qCDebug(appLog) << "Matched DJVU file type";
        fileType = DJVU;
    
#ifdef XPS_SUPPORT_ENABLED
    } else if (filePath.right(4).toLower() == ".xps") {
        qCDebug(appLog) << "Matched XPS file type by extension";
        fileType = XPS;
    } else if (mimeType.name() == QLatin1String("application/vnd.ms-xpsdocument")) {
        qCDebug(appLog) << "Matched XPS file type by MIME type";
        fileType = XPS;
#endif
    } else if (mimeType.name() == QLatin1String("application/zip") && filePath.right(4) == "pptx") {
        qCDebug(appLog) << "Matched PPTX file type";
        fileType = PPTX;
    } else if (mimeType.name() == QLatin1String("application/zip") && filePath.right(4) == "docx") {
        qCDebug(appLog) << "Matched DOCX file type (zip)";
        fileType = DOCX;
    } else if (mimeType.name() == QLatin1String("application/x-ole-storage") && filePath.right(4) == "docx") {
        qCDebug(appLog) << "Matched DOCX file type (ole)";
        fileType = DOCX;
    } else {
        qCDebug(appLog) << "Unknown file type";
    }

    qCDebug(appLog) << "Final file type:" << static_cast<int>(fileType);
    return fileType;
}

}

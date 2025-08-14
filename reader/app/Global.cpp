// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Global.h"

#include <QMimeType>
#include <QMimeDatabase>
#include <QDebug>
#include <DGuiApplicationHelper>

namespace Dr {
FileType fileType(const QString &filePath)
{
    qDebug() << "Detecting file type for:" << filePath;
    FileType fileType = FileType::Unknown;

    //具体的MIME文件类型可以参考https://baike.baidu.com/item/MIME/2900607?fr=aladdin
    const QMimeType mimeType = QMimeDatabase().mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

    qInfo() << "File MIME type:" << mimeType.name();
    qDebug() << "Matching file type...";
    if (mimeType.name() == QLatin1String("application/pdf")) {
        qDebug() << "Matched PDF file type";
        fileType = PDF;
    } else if (mimeType.name() == QLatin1String("application/postscript")) {
        qDebug() << "Matched PS file type";
        fileType = PS;
    } else if (mimeType.name() == QLatin1String("image/vnd.djvu") || mimeType.name() == QLatin1String("image/vnd.djvu+multipage")) {
        qDebug() << "Matched DJVU file type";
        fileType = DJVU;
    
#ifdef XPS_SUPPORT_ENABLED
    } else if (filePath.right(4).toLower() == ".xps") {
        qDebug() << "Matched XPS file type by extension";
        fileType = XPS;
    } else if (mimeType.name() == QLatin1String("application/vnd.ms-xpsdocument")) {
        qDebug() << "Matched XPS file type by MIME type";
        fileType = XPS;
#endif
    } else if (mimeType.name() == QLatin1String("application/zip") && filePath.right(4) == "pptx") {
        qDebug() << "Matched PPTX file type";
        fileType = PPTX;
    } else if (mimeType.name() == QLatin1String("application/zip") && filePath.right(4) == "docx") {
        qDebug() << "Matched DOCX file type (zip)";
        fileType = DOCX;
    } else if (mimeType.name() == QLatin1String("application/x-ole-storage") && filePath.right(4) == "docx") {
        qDebug() << "Matched DOCX file type (ole)";
        fileType = DOCX;
    } else {
        qDebug() << "Unknown file type";
    }

    qDebug() << "Final file type:" << static_cast<int>(fileType);
    return fileType;
}

}

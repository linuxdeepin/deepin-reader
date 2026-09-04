// Copyright (C) 2019 - 2026 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 -2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Global.h"
#include "ddlog.h"

#include <QMimeType>
#include <QMimeDatabase>
#include <QDebug>
#include <QStorageInfo>
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

bool isNetworkPath(const QString &filePath)
{
    // gvfs（smb/nfs 等用户态挂载）路径特征明确，先按字符串判断。
    // 挂载断开后目录不存在，QStorageInfo 会返回无效而被误判为本地路径，
    // 导致后续状态清理逻辑无法按网络文档的超时策略处理
    if (filePath.startsWith("/run/user/") && filePath.contains("/gvfs/"))
        return true;

    const QStorageInfo storage(filePath);
    if (!storage.isValid() || storage.isRoot())
        return false;

    const QByteArray fsType = storage.fileSystemType().toLower();
    if (fsType.contains("nfs") || fsType.contains("cifs") ||
        fsType.contains("smb") || fsType.contains("sshfs") ||
        fsType.contains("gvfsd"))
        return true;

    return false;
}

}

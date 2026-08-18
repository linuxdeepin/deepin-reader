// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "BatchPrintHandler.h"
#include "Global.h"
#include "ddlog.h"

#include <QFileInfo>
#include <QProcess>
#include <QDir>
#include <QDebug>

// 系统敏感目录黑名单，符号链接指向这些目录时拒绝打印
static const QStringList kProtectedDirs = {
    "/etc", "/proc", "/sys", "/dev", "/boot", "/root", "/var/log",
    "/bin", "/sbin", "/lib", "/lib64", "/lib32", "/libx32",
    "/usr/bin", "/usr/sbin", "/usr/lib", "/usr/lib64", "/usr/lib32",
    "/usr/local/bin", "/usr/local/sbin", "/usr/local/lib",
    "/usr/share", "/usr/include",
    "/opt", "/srv", "/run", "/snap"
};

/*
 * 校验文件路径是否安全：
 * 1. 解析符号链接，获取真实路径
 * 2. 拒绝指向系统敏感目录的符号链接
 * 3. 返回解析后的安全路径（空字符串表示不安全）
 */
static QString resolveSafeFilePath(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    QString canonicalPath = fileInfo.canonicalFilePath();

    // 符号链接断裂或文件不存在
    if (canonicalPath.isEmpty()) {
        qCWarning(appLog) << "Path cannot be resolved (broken symlink?):" << filePath;
        return QString();
    }

    // 检查真实路径是否指向系统敏感目录
    for (const QString &protectedDir : kProtectedDirs) {
        if (canonicalPath.startsWith(protectedDir + "/") || canonicalPath == protectedDir) {
            qCWarning(appLog) << "Refusing to print file in protected directory:"
                              << filePath << "->" << canonicalPath;
            return QString();
        }
    }

    // 拒绝访问 .ssh 目录下的任何文件（SSH 密钥/配置）
    if (canonicalPath.contains("/.ssh/")) {
        qCWarning(appLog) << "Refusing to print file in .ssh directory:"
                          << filePath << "->" << canonicalPath;
        return QString();
    }

    return canonicalPath;
}

int BatchPrintHandler::runBatchPrint(const QStringList &filePaths)
{
    qCInfo(appLog) << "Batch print started, file count:" << filePaths.size();

    if (filePaths.isEmpty()) {
        qCWarning(appLog) << "No files provided for batch print";
        return 1;
    }

    int successCount = 0;
    int failCount = 0;
    int skippedCount = 0;

    for (const QString &filePath : filePaths) {
        if (!isSupportedFormat(filePath)) {
            qCInfo(appLog) << "Skipping unsupported format:" << filePath;
            skippedCount++;
            continue;
        }

        QString safePath = resolveSafeFilePath(filePath);
        if (safePath.isEmpty()) {
            qCWarning(appLog) << "File path is not safe:" << filePath;
            failCount++;
            continue;
        }

        QFileInfo fileInfo(safePath);
        if (!fileInfo.exists() || !fileInfo.isReadable()) {
            qCWarning(appLog) << "File not accessible:" << filePath;
            failCount++;
            continue;
        }

        if (printDocument(safePath)) {
            successCount++;
        } else {
            qCWarning(appLog) << "Failed to print:" << filePath;
            failCount++;
        }
    }

    qCInfo(appLog) << "Batch print completed: success=" << successCount
                    << "fail=" << failCount << "skipped=" << skippedCount;

    return (successCount == 0) ? 1 : ((failCount > 0) ? 1 : 0);
}

/*
 * 通过文件头魔数校验文件真实格式，防止将任意文件重命名为 .pdf 等后缀绕过检查。
 * 返回 true 表示文件头与后缀声明的格式一致。
 */
static bool checkMagicNumber(const QString &filePath, const QString &suffix)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // 读取前 16 字节用于魔数检测
    QByteArray header = file.read(16);
    file.close();

    if (header.size() < 4) {
        return false;
    }

    // PDF: 以 "%PDF-" 开头 (5 字节)
    if (suffix == "pdf") {
        return header.startsWith("%PDF-");
    }

    // DJVU: 以 "AT&TFORM" 开头 (8 字节)
    if (suffix == "djvu") {
        return header.startsWith("AT&TFORM");
    }

    // DOCX/XPS: 均为 ZIP 容器，以 "PK\x03\x04" 开头 (4 字节)
    if (suffix == "docx" || suffix == "xps") {
        return header.startsWith("PK\x03\x04");
    }

    return false;
}

bool BatchPrintHandler::isSupportedFormat(const QString &filePath)
{
    QString suffix = QFileInfo(filePath).suffix().toLower();
    // 支持的格式：pdf, djvu, docx, xps
    if (suffix != "pdf" && suffix != "djvu" && suffix != "docx" && suffix != "xps") {
        return false;
    }

    // 校验文件头魔数，防止任意文件重命名后缀绕过格式检查
    return checkMagicNumber(filePath, suffix);
}

bool BatchPrintHandler::printDocument(const QString &filePath)
{
    qCDebug(appLog) << "Printing document:" << filePath;
    return printWithLp(filePath);
}

bool BatchPrintHandler::printWithLp(const QString &filePath)
{
    qCDebug(appLog) << "Submitting print job via lp:" << filePath;

    static const QString lpBinPath = QStringLiteral("/usr/bin/lp");
    QProcess process;
    // -o fit-to-page: 自动缩放到纸张大小
    // -o sides=one-sided: 默认单面打印
    // -- 分隔选项与文件路径，防止以 '-' 开头的文件名被解析为 lp 选项
    QStringList args;
    args << "-o" << "fit-to-page"
         << "-o" << "sides=one-sided"
         << "--"
         << filePath;

    process.start(lpBinPath, args);
    if (!process.waitForFinished(5000)) {
        qCWarning(appLog) << "lp command timeout for:" << filePath;
        process.kill();
        process.waitForFinished(2000);
        return false;
    }

    int exitCode = process.exitCode();
    if (exitCode != 0) {
        QString stderr = process.readAllStandardError();
        qCWarning(appLog) << "lp command failed for:" << filePath
                           << "exitCode:" << exitCode << "error:" << stderr;
        return false;
    }

    qCDebug(appLog) << "Print job submitted successfully for:" << filePath;
    return true;
}

// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Model.h"
#ifdef XPS_SUPPORT_ENABLED
#include "XpsDocument.h"
#endif
#include "PDFModel.h"
#include "DjVuModel.h"
#include "dpdfannot.h"
#include "dpdfpage.h"
#include "dpdfdoc.h"
#include "ddlog.h"

#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QTemporaryFile>
#include <QLibraryInfo>


namespace {

QString getHtmlToPdfPath() {

    QString path = QString(INSTALL_PREFIX) + "/lib/deepin-reader/htmltopdf";
    if (QFile::exists(path)) {
        qCDebug(appLog) << "Found htmltopdf in INSTALL_PREFIX: " << path;
        return path;
    }
#if (QT_VERSION > QT_VERSION_CHECK(6, 0, 0))
    path = QLibraryInfo::path(QLibraryInfo::LibrariesPath) + "/deepin-reader/htmltopdf";
#else
    path = QLibraryInfo::location(QLibraryInfo::LibrariesPath) + "/deepin-reader/htmltopdf";
#endif

    if (QFile::exists(path)) {
        qCDebug(appLog) << "Found htmltopdf in LibrariesPath: " << path;
        return path;
    }

    qCWarning(appLog) << "Not found htmltopdf";
    return QString();
}

}
namespace deepin_reader {
deepin_reader::Document *deepin_reader::DocumentFactory::getDocument(const int &fileType,
                                                                     const QString &filePath,
                                                                     const QString &convertedFileDir,
                                                                     const QString &password,
                                                                     QProcess **pprocess,
                                                                     deepin_reader::Document::Error &error)
{
    qCDebug(appLog) << "Creating document handler for type:" << fileType << "file:" << filePath;

    deepin_reader::Document *document = nullptr;

    qCDebug(appLog) << "Processing document file:" << filePath;
    if (Dr::PDF == fileType) {
        qCDebug(appLog) << "Handling PDF document";
        document = deepin_reader::PDFDocument::loadDocument(filePath, password, error);
    } else if (Dr::DJVU == fileType) {
        qCDebug(appLog) << "Handling DJVU document";
        document = deepin_reader::DjVuDocument::loadDocument(filePath, error);
#ifdef XPS_SUPPORT_ENABLED
    } else if (Dr::XPS == fileType) {
        qCDebug(appLog) << "Handling XPS document";
        document = deepin_reader::XpsDocument::loadDocument(filePath, error);
#endif
    } else if (Dr::DOCX == fileType) {
        qCDebug(appLog) << "Starting DOCX document conversion process";
        if (nullptr == pprocess) {
            qCritical() << "Invalid process pointer for DOCX conversion";
            error = deepin_reader::Document::ConvertFailed;
            return nullptr;
        }
        QString targetDoc = convertedFileDir + "/temp.docx";
        QString tmpHtmlFilePath = convertedFileDir + "/word/temp.html";
        QString realFilePath = convertedFileDir + "/temp.pdf";

        // QDir convertedDir(convertedFileDir);
        // if (!convertedDir.exists()) {
        //     bool flag = convertedDir.mkdir(convertedFileDir);
        //     qCDebug(appLog) << "创建文件夹" << convertedFileDir << "是否成功？" << flag;
        // } else {
        //     qCDebug(appLog) << "文件夹" << convertedFileDir << "已经存在！";
        // }
        // qCDebug(appLog) << "targetDoc: " << targetDoc;
        // qCDebug(appLog) << "tmpHtmlFilePath: " << tmpHtmlFilePath;
        // qCDebug(appLog) << "realFilePath: " << realFilePath;

        QString prefix = INSTALL_PREFIX;
        // qCDebug(appLog) << "应用安装路径: " << prefix;

        QFile file(filePath);
        if (!file.copy(targetDoc)) {
            qCritical() << "Failed to copy file from" << filePath << "to" << targetDoc;
            error = deepin_reader::Document::ConvertFailed;
            return nullptr;
        }

        //解压的目的是为了让资源文件可以被转换的时候使用到，防止转换后丢失图片等媒体信息
        QProcess decompressor;
        *pprocess = &decompressor;
        decompressor.setWorkingDirectory(convertedFileDir);
        qCInfo(appLog) << "Unzipping DOCX document:" << targetDoc;
        QString unzipCommand = "unzip " + targetDoc;
        qCDebug(appLog) << "执行命令: " << unzipCommand;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        decompressor.start(unzipCommand);
#else
        decompressor.startCommand(unzipCommand);
#endif
        if (!decompressor.waitForStarted()) {
            qCritical() << "Failed to start unzip process for file:" << targetDoc;
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        if (!decompressor.waitForFinished()) {
            qCritical() << "Unzip process failed for file:" << targetDoc;
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        if (!QDir(convertedFileDir + "/word").exists()) {
            qCritical() << "Unzip failed! Directory not found:" << (convertedFileDir + "/word");
            error = deepin_reader::Document::ConvertFailed;
            if (!(QProcess::CrashExit == decompressor.exitStatus() && 9 == decompressor.exitCode())) {
                *pprocess = nullptr;
            }
            return nullptr;
        }
        qCInfo(appLog) << "Document unzipped successfully";
        QTemporaryFile tmpFile(convertedFileDir + "/word/" + QCoreApplication::applicationName() + "_XXXXXX.html");
        if( tmpFile.open()) { //fix 232871
             tmpHtmlFilePath = tmpFile.fileName(); // returns the unique file name
         }
        // docx -> html
        QProcess converter;
        *pprocess = &converter;
        converter.setWorkingDirectory(convertedFileDir + "/word");
        qCInfo(appLog) << "Converting DOCX to HTML:" << tmpHtmlFilePath;
        // QFile targetDocFile(targetDoc);
        // if (targetDocFile.exists()) {
        //     qCDebug(appLog) << "文档" << targetDocFile.fileName() << "存在！";
        // } else {
        //     qCDebug(appLog) << "文档" << targetDocFile.fileName() << "不存在！";
        // }
        QString pandocDataDir = prefix + "/share/pandoc/data";
        QString pandocCommand = QString("pandoc %1 --data-dir=%2 -o %3").arg(targetDoc).arg(pandocDataDir).arg(tmpHtmlFilePath);

        qCDebug(appLog) << "执行命令: " << pandocCommand;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        converter.start(pandocCommand);
#else
        converter.startCommand(pandocCommand);
#endif
        if (!converter.waitForStarted()) {
            qCritical() << "Failed to start pandoc conversion process";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        if (!converter.waitForFinished()) {
            qCritical() << "Pandoc conversion process failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        QFile tmpHtmlFile(tmpHtmlFilePath);
        if (!tmpHtmlFile.exists()) {
            qCritical() << "Pandoc conversion failed! Output file not found:" << tmpHtmlFilePath;
            error = deepin_reader::Document::ConvertFailed;
            // 转换过程中关闭应用，docsheet被释放，对应的*pprocess已不存在
            if (!(QProcess::CrashExit == converter.exitStatus() && 9 == converter.exitCode())) {
                *pprocess = nullptr;
            }
            return nullptr;
        }
        qCInfo(appLog) << "DOCX to HTML conversion completed";

        // html -> pdf
        QProcess converter2;
        *pprocess = &converter2;
        converter2.setWorkingDirectory(convertedFileDir + "/word");
        qCInfo(appLog) << "Converting HTML to PDF:" << realFilePath;

        QString htmltopdfCommand = getHtmlToPdfPath() + " " + tmpHtmlFilePath + " " + realFilePath;
        qCDebug(appLog) << "执行命令: " << htmltopdfCommand;
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        converter2.start(htmltopdfCommand);
#else
        converter2.startCommand(htmltopdfCommand);
#endif
        if (!converter2.waitForStarted()) {
            qCritical() << "Failed to start htmltopdf conversion process";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        if (!converter2.waitForFinished()) {
            qCritical() << "Htmltopdf conversion process failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }

        QFile realFile(realFilePath);
        if (!realFile.exists()) {
            qCritical() << "Htmltopdf conversion failed! Output file not found:" << realFilePath;
            error = deepin_reader::Document::ConvertFailed;
            if (!(QProcess::CrashExit == converter.exitStatus() && 9 == converter.exitCode())) {
                *pprocess = nullptr;
            }
            return nullptr;
        }
        qCInfo(appLog) << "HTML to PDF conversion completed";

        *pprocess = nullptr;
        qCInfo(appLog) << "Loading converted PDF document:" << realFilePath;
        document = deepin_reader::PDFDocument::loadDocument(realFilePath, password, error);
    }

    if (document) {
        qCInfo(appLog) << "Document created successfully for:" << filePath;
    } else {
        qCWarning(appLog) << "Failed to create document for:" << filePath << "Error:" << error;
    }
    return document;
}

bool SearchResult::setctionsFillText(std::function<QString(int, QRectF)> getText)
{
    qCDebug(appLog) << "Filling search result text for page:" << page;

    bool ret = false;
    for (auto &section : sections) {
        for (auto &line : section) {
            //这里的page比index大1
            int index = page - 1;
            QString text = getText(index, line.rect);
            if (!text.isEmpty()) {
                line.text = text;
                ret = true;
            }
        }
    }
    qCDebug(appLog) << "Search result text filled, success:" << ret;
    return ret;
}

QRectF SearchResult::sectionBoundingRect(const PageSection &section)
{
    qCDebug(appLog) << "Calculating bounding rect for search result section";

    QRectF ret;
    for (const PageLine &line : section) {
        ret = ret.united(line.rect);
    }
    return ret;
}

}

// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "Model.h"
#include "PDFModel.h"
#include "DjVuModel.h"
#include "dpdfannot.h"
#include "dpdfpage.h"
#include "dpdfdoc.h"

#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QTemporaryFile>

namespace deepin_reader {
deepin_reader::Document *deepin_reader::DocumentFactory::getDocument(const int &fileType,
                                                                     const QString &filePath,
                                                                     const QString &convertedFileDir,
                                                                     const QString &password,
                                                                     QProcess **pprocess,
                                                                     deepin_reader::Document::Error &error)
{
    qDebug() << "Creating document handler for type:" << fileType << "file:" << filePath;

    deepin_reader::Document *document = nullptr;

    qDebug() << "Processing document file:" << filePath;
    if (Dr::PDF == fileType) {
        qDebug() << "Handling PDF document";
        document = deepin_reader::PDFDocument::loadDocument(filePath, password, error);
    } else if (Dr::DJVU == fileType) {
        qDebug() << "Handling DJVU document";
        document = deepin_reader::DjVuDocument::loadDocument(filePath, error);
    } else if (Dr::DOCX == fileType) {
        qDebug() << "Starting DOCX document conversion process";
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
        //     qDebug() << "创建文件夹" << convertedFileDir << "是否成功？" << flag;
        // } else {
        //     qDebug() << "文件夹" << convertedFileDir << "已经存在！";
        // }
        // qDebug() << "targetDoc: " << targetDoc;
        // qDebug() << "tmpHtmlFilePath: " << tmpHtmlFilePath;
        // qDebug() << "realFilePath: " << realFilePath;

        QString prefix = INSTALL_PREFIX;
        // qDebug() << "应用安装路径: " << prefix;

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
        qInfo() << "Unzipping DOCX document:" << targetDoc;
        QString unzipCommand = "unzip " + targetDoc;
        qDebug() << "执行命令: " << unzipCommand;
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
        qInfo() << "Document unzipped successfully";
        QTemporaryFile tmpFile(convertedFileDir + "/word/" + QCoreApplication::applicationName() + "_XXXXXX.html");
        if( tmpFile.open()) { //fix 232871
             tmpHtmlFilePath = tmpFile.fileName(); // returns the unique file name
         }
        // docx -> html
        QProcess converter;
        *pprocess = &converter;
        converter.setWorkingDirectory(convertedFileDir + "/word");
        qInfo() << "Converting DOCX to HTML:" << tmpHtmlFilePath;
        // QFile targetDocFile(targetDoc);
        // if (targetDocFile.exists()) {
        //     qDebug() << "文档" << targetDocFile.fileName() << "存在！";
        // } else {
        //     qDebug() << "文档" << targetDocFile.fileName() << "不存在！";
        // }
        QString pandocDataDir = prefix + "/share/pandoc/data";
        QString pandocCommand = QString("pandoc %1 --data-dir=%2 -o %3").arg(targetDoc).arg(pandocDataDir).arg(tmpHtmlFilePath);

        qDebug() << "执行命令: " << pandocCommand;
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
        qInfo() << "DOCX to HTML conversion completed";

        // html -> pdf
        QProcess converter2;
        *pprocess = &converter2;
        converter2.setWorkingDirectory(convertedFileDir + "/word");
        qInfo() << "Converting HTML to PDF:" << realFilePath;

        QString htmltopdfCommand = prefix + "/lib/deepin-reader/htmltopdf " +  tmpHtmlFilePath + " " + realFilePath;
        qDebug() << "执行命令: " << htmltopdfCommand;
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
        qInfo() << "HTML to PDF conversion completed";

        *pprocess = nullptr;
        qInfo() << "Loading converted PDF document:" << realFilePath;
        document = deepin_reader::PDFDocument::loadDocument(realFilePath, password, error);
    }

    if (document) {
        qInfo() << "Document created successfully for:" << filePath;
    } else {
        qWarning() << "Failed to create document for:" << filePath << "Error:" << error;
    }
    return document;
}

bool SearchResult::setctionsFillText(std::function<QString(int, QRectF)> getText)
{
    qDebug() << "Filling search result text for page:" << page;

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
    qDebug() << "Search result text filled, success:" << ret;
    return ret;
}

QRectF SearchResult::sectionBoundingRect(const PageSection &section)
{
    qDebug() << "Calculating bounding rect for search result section";

    QRectF ret;
    for (const PageLine &line : section) {
        ret = ret.united(line.rect);
    }
    return ret;
}

}

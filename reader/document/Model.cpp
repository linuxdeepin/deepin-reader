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
    deepin_reader::Document *document = nullptr;

    qDebug() << "需要转换的文档: " << filePath;
    if (Dr::PDF == fileType) {
        qDebug() << "当前文档类型为: PDF";
        document = deepin_reader::PDFDocument::loadDocument(filePath, password, error);
    } else if (Dr::DJVU == fileType) {
        qDebug() << "当前文档类型为: DJVU";
        document = deepin_reader::DjVuDocument::loadDocument(filePath, error);
    } else if (Dr::DOCX == fileType) {
        qDebug() << "当前文档类型为: DOCX";
        if (nullptr == pprocess) {
            error = deepin_reader::Document::ConvertFailed;
            return nullptr;
        }
        QString targetDoc = convertedFileDir + "/temp.docx";
        QString tmpHtmlFilePath = convertedFileDir + "/word/temp.html";
        QString realFilePath = convertedFileDir + "/temp.pdf";

        QFile file(filePath);
        if (!file.copy(targetDoc)) {
            qInfo() << QString("copy %1 failed.").arg(filePath);
            error = deepin_reader::Document::ConvertFailed;
            return nullptr;
        }

        //解压的目的是为了让资源文件可以被转换的时候使用到，防止转换后丢失图片等媒体信息
        QProcess decompressor;
        *pprocess = &decompressor;
        decompressor.setWorkingDirectory(convertedFileDir);
        qDebug() << "正在解压文档..." << targetDoc;
        QString unzipCommand = "unzip " + targetDoc;
        qDebug() << "执行命令: " << unzipCommand;
        decompressor.start(unzipCommand);
        if (!decompressor.waitForStarted()) {
            qInfo() << "start unzip failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        if (!decompressor.waitForFinished()) {
            qInfo() << "unzip failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        if (!QDir(convertedFileDir + "/word").exists()) {
            qInfo() << "unzip failed! " << (convertedFileDir + "/word") << "is not exists!";
            error = deepin_reader::Document::ConvertFailed;
            if (!(QProcess::CrashExit == decompressor.exitStatus() && 9 == decompressor.exitCode())) {
                *pprocess = nullptr;
            }
            return nullptr;
        }
        qDebug() << "文档解压完成";
        QTemporaryFile tmpFile(convertedFileDir + "/word/" + QCoreApplication::applicationName() + "_XXXXXX.html");
        if( tmpFile.open()) { //fix 232871
             tmpHtmlFilePath = tmpFile.fileName(); // returns the unique file name
         }
        // docx -> html
        QProcess converter;
        *pprocess = &converter;
        converter.setWorkingDirectory(convertedFileDir + "/word");
        qDebug() << "正在将docx文档转换成html..." << tmpHtmlFilePath;
        QString pandocCommand = "pandoc " +  targetDoc + " -o " + tmpHtmlFilePath;
        qDebug() << "执行命令: " << pandocCommand;
        converter.start(pandocCommand);
        if (!converter.waitForStarted()) {
            qInfo() << "start pandoc failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        if (!converter.waitForFinished()) {
            qInfo() << "pandoc failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        QFile tmpHtmlFile(tmpHtmlFilePath);
        if (!tmpHtmlFile.exists()) {
            qInfo() <<  "pandoc failed! " << tmpHtmlFilePath << " doesn't exist";
            error = deepin_reader::Document::ConvertFailed;
            // 转换过程中关闭应用，docsheet被释放，对应的*pprocess已不存在
            if (!(QProcess::CrashExit == converter.exitStatus() && 9 == converter.exitCode())) {
                *pprocess = nullptr;
            }
            return nullptr;
        }
        qDebug() << "docx转html完成";

        // html -> pdf
        QProcess converter2;
        *pprocess = &converter2;
        converter2.setWorkingDirectory(convertedFileDir + "/word");
        qDebug() << "正在将html转换成pdf..." << realFilePath;
        QString htmltopdfCommand = "/usr/lib/deepin-reader/htmltopdf " +  tmpHtmlFilePath + " " + realFilePath;
        qDebug() << "执行命令: " << htmltopdfCommand;
        converter2.start(htmltopdfCommand);
        if (!converter2.waitForStarted()) {
            qInfo() << "start htmltopdf failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        if (!converter2.waitForFinished()) {
            qInfo() << "htmltopdf failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }

        QFile realFile(realFilePath);
        if (!realFile.exists()) {
            // htmltopdf 依赖 QtWebEngine，在部分架构（如 sw_64）上 webengine 内部崩溃会导致 pdf 无法生成。
            // 兜底使用 libreoffice 直接将 docx 转换为 pdf，绕开 webengine，恢复 DOCX 文档的可用性。
            qInfo() << "htmltopdf failed!" << realFilePath << "doesn't exist, fallback to libreoffice";
            QProcess fallback;
            *pprocess = &fallback;
            fallback.setWorkingDirectory(convertedFileDir);
            // 独立的 UserInstallation 避免与已运行的 libreoffice 抢占用户配置
            QString fallbackCommand =
                "libreoffice --headless -env:UserInstallation=file://" + convertedFileDir + "/lo_profile"
                + " --convert-to pdf --outdir " + convertedFileDir + " " + targetDoc;
            qDebug() << "执行命令: " << fallbackCommand;
            fallback.start(fallbackCommand);
            if (!fallback.waitForStarted()) {
                qInfo() << "start libreoffice fallback failed";
                error = deepin_reader::Document::ConvertFailed;
                *pprocess = nullptr;
                return nullptr;
            }
            // libreoffice 为重量级套件，每次兜底均使用全新 lo_profile 冷启动，叠加 sw_64 较慢，
            // 大文档/带图表 docx 转换可能超过 QProcess 默认 30s 超时，此处放宽至 180s 避免误判失败。
            if (!fallback.waitForFinished(180000)) {
                qInfo() << "libreoffice fallback failed";
                error = deepin_reader::Document::ConvertFailed;
                *pprocess = nullptr;
                return nullptr;
            }
            if (!realFile.exists()) {
                qInfo() << "libreoffice fallback failed!" << realFilePath << "doesn't exist";
                error = deepin_reader::Document::ConvertFailed;
                // 转换过程中关闭应用，docsheet 被释放，对应的 *pprocess 已不存在；
                // 仅当进程非被 SIGKILL（用户关闭文档）时才置空，避免对已释放内存写入。
                if (!(QProcess::CrashExit == fallback.exitStatus() && 9 == fallback.exitCode())) {
                    *pprocess = nullptr;
                }
                return nullptr;
            }
            // 兜底成功：在 fallback 析构前置空 pprocess，避免悬挂指针。
            // 此处与下方函数作用域的置空非冗余：此处服务块作用域的 fallback（析构前清空），
            // 下方服务函数作用域的 converter2（此时仍存活）。
            *pprocess = nullptr;
        }
        qDebug() << "docx转pdf完成";

        *pprocess = nullptr;
        document = deepin_reader::PDFDocument::loadDocument(realFilePath, password, error);
    }

    return document;
}

bool SearchResult::setctionsFillText(std::function<QString(int, QRectF)> getText)
{
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
    return ret;
}

QRectF SearchResult::sectionBoundingRect(const PageSection &section)
{
    QRectF ret;
    for (const PageLine &line : section) {
        ret = ret.united(line.rect);
    }
    return ret;
}

}

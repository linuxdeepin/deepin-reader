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
        deepin_reader::PDFDocument *pdfDoc = nullptr;
        pdfDoc = deepin_reader::PDFDocument::loadDocument(filePath, password, error);
        QFileInfo fileInfo(filePath);
        if (pdfDoc) {
            pdfDoc->setPath(fileInfo.absolutePath(), "", 0);
        }
        document = pdfDoc;
    } else if (Dr::DJVU == fileType) {
        document = deepin_reader::DjVuDocument::loadDocument(filePath, error);
    } else if (Dr::DOCX == fileType) {
        if (nullptr == pprocess) {
            error = deepin_reader::Document::ConvertFailed;
            return nullptr;
        }
        //目标文档（需要进行转换的文档）
        QString targetDoc = convertedFileDir + "/temp.docx";
        //工作目录（unzip解压后的目录）
        QString tmpWorkDir = convertedFileDir + "/word";
        //最终文档（文档查看器需要的文档）
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
        QString command = "unzip " + targetDoc;
        qInfo() << "正在执行 (" << targetDoc << ")文档解压 ..." << command;
        decompressor.start(command);
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
        if (!QDir(tmpWorkDir).exists()) {
            qInfo() << "unzip failed";
            error = deepin_reader::Document::ConvertFailed;
            if (!(QProcess::CrashExit == decompressor.exitStatus() && 9 == decompressor.exitCode())) {
                *pprocess = nullptr;
            }
            return nullptr;
        }
        qInfo() << "(" << targetDoc << ")文档解压 已完成";

        QFileInfo tmpWorkDirInfo(tmpWorkDir);
        qDebug() << "工作目录(" << tmpWorkDir << ")的文件权限(r-w-e): " << tmpWorkDirInfo.isReadable() << tmpWorkDirInfo.isWritable() << tmpWorkDirInfo.isExecutable();

        //碰到unzip后的没有权限的目录，将工作目录重置
        if (!tmpWorkDirInfo.isReadable() && !tmpWorkDirInfo.isWritable()) {
            qDebug() << "工作目录没有读写权限,跳过解压缩步骤！";
            tmpWorkDir = convertedFileDir;
        }

        //临时文档（通过pandoc转换出来的html文档）
        QString tmpHtmlFilePath = tmpWorkDir + "/temp.html";

        // docx -> html
        QProcess converter;
        *pprocess = &converter;
        converter.setWorkingDirectory(tmpWorkDir);
        command = "pandoc " +  targetDoc + " -o " + tmpHtmlFilePath;
        qInfo() << "正在执行 docx -> html ...";
        converter.start(command);
        qDebug() << "docx -> html: " << command;
        if (!converter.waitForStarted()) {
            qInfo() << "start pandoc failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        //由于有些文档执行转换的时间过长，需要延长等待时间
        if (!converter.waitForFinished(2000000)) {
            qInfo() << "pandoc failed";
            error = deepin_reader::Document::ConvertFailed;
            *pprocess = nullptr;
            return nullptr;
        }
        QFile tmpHtmlFile(tmpHtmlFilePath);
        if (!tmpHtmlFile.exists()) {
            qInfo() << "temp.html doesn't exist";
            error = deepin_reader::Document::ConvertFailed;
            // 转换过程中关闭应用，docsheet被释放，对应的*pprocess已不存在
            if (!(QProcess::CrashExit == converter.exitStatus() && 9 == converter.exitCode())) {
                *pprocess = nullptr;
            }
            return nullptr;
        }
        qInfo() << "docx -> html 已完成";
        // html -> pdf
        QProcess converter2;
        *pprocess = &converter2;
        converter2.setWorkingDirectory(tmpWorkDir);
        qInfo() << "正在执行 html -> pdf ...";
        command = "/usr/lib/deepin-reader/htmltopdf " +  tmpHtmlFilePath + " " + realFilePath;
        converter2.start(command);
        qDebug() << "html -> pdf: " << command;
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
            qInfo() << "temp.pdf doesn't exist";
            error = deepin_reader::Document::ConvertFailed;
            if (!(QProcess::CrashExit == converter.exitStatus() && 9 == converter.exitCode())) {
                *pprocess = nullptr;
            }
            return nullptr;
        }

        qInfo() << "html -> pdf 已完成";
        *pprocess = nullptr;
        deepin_reader::PDFDocument *pdfDoc = nullptr;
        pdfDoc = deepin_reader::PDFDocument::loadDocument(realFilePath, password, error);
        QFileInfo fileInfo(filePath);

        if (pdfDoc) {
            pdfDoc->setPath(fileInfo.absolutePath(), convertedFileDir, 1);
        }
        document = pdfDoc;
    }

    return document;
}

}

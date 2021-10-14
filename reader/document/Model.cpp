#include "Model.h"
#include "PDFModel.h"
#include "DjVuModel.h"
#include "dpdfannot.h"
#include "dpdfpage.h"
#include "dpdfdoc.h"
#include "mythread.h"
#include "html2pdfconverter.h"

#include <QProcess>
#include <QFile>
#include <QDir>
#include <QTimer>

namespace deepin_reader {
deepin_reader::Document *deepin_reader::DocumentFactory::getDocument(const int &fileType,
                                                                     const QString &filePath,
                                                                     const QString &convertedFileDir,
                                                                     const QString &password,
                                                                     deepin_reader::Document::Error &error)
{
    deepin_reader::Document *document = nullptr;

    if (Dr::PDF == fileType) {
        document = deepin_reader::PDFDocument::loadDocument(filePath, password, error);
    } else if (Dr::DJVU == fileType) {
        document = deepin_reader::DjVuDocument::loadDocument(filePath, error);
    } else if (Dr::DOCX == fileType) {
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
        decompressor.setWorkingDirectory(convertedFileDir);
        decompressor.start("unzip " + targetDoc);
        decompressor.waitForStarted();
        decompressor.waitForFinished();

        QProcess converter;
        converter.setWorkingDirectory(convertedFileDir + "/word");
        converter.start("pandoc " +  targetDoc + " -o " + tmpHtmlFilePath);
        converter.waitForStarted();
        converter.waitForFinished();

        QString oldPath = QDir::currentPath();
        QDir::setCurrent(convertedFileDir + "/word");
        MyThread mythread(tmpHtmlFilePath, realFilePath);
        mythread.doHtml2PdfConverterWork();
        QDir::setCurrent(oldPath);

        QFile realFile(realFilePath);

        if (!realFile.exists()) {
            error = deepin_reader::Document::ConvertFailed;
            return nullptr;
        }

        document = deepin_reader::PDFDocument::loadDocument(realFilePath, password, error);
    }

    return document;
}

}

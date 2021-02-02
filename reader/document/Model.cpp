#include "Model.h"
#include "PDFModel.h"
#include "DjVuModel.h"

#include <QTemporaryDir>
#include <QProcess>

namespace deepin_reader {
deepin_reader::Document *deepin_reader::DocumentFactory::getDocument(const int &fileType, const QString &filePath, const QString &password)
{
    deepin_reader::Document *document = nullptr;

    if (Dr::PDF == fileType)
        document = deepin_reader::PDFDocument::loadDocument(filePath, password);
    else if (Dr::DJVU == fileType)
        document = deepin_reader::DjVuDocument::loadDocument(filePath);
    else if (Dr::DOCX == fileType) {
        QTemporaryDir dir;
        QString targetDoc = dir.path() + "/temp.docx";
        QString targetPdf = dir.path() + "/temp.pdf";

        QFile file(filePath);
        file.copy(targetDoc);

        //解压的目的是为了让资源文件可以被转换的时候使用到，防止转换后丢失图片等媒体信息
        QProcess Decompressor;
        Decompressor.setWorkingDirectory(dir.path());
        Decompressor.start("unzip " + targetDoc);
        Decompressor.waitForStarted();
        Decompressor.waitForFinished();

        QProcess converter;
        converter.setWorkingDirectory(dir.path() + "/word");
        converter.start("pandoc " +  targetDoc + " -o " + targetPdf + " --pdf-engine=wkhtmltopdf -V CJKmainfont=\"Noto Sans CJK JP - Bold\"");
        converter.waitForStarted();
        converter.waitForFinished();

        document = deepin_reader::PDFDocument::loadDocument(targetPdf, password);
    }

    return document;
}

}

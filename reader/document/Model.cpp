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
        //以下后期挪到一个转化对话框中，实时打印输出
        QTemporaryDir dir;
        QString targetDoc = dir.path() + "/temp.docx";
        QString targetPdf = dir.path() + "/temp.pdf";

        QFile file(filePath);
        file.copy(targetDoc);

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

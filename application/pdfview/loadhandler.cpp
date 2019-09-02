#include "loadhandler.h"
#include <QMimeDatabase>
#include <QDebug>
#include <QFileInfo>
#include <QImageReader>
//tmp
#include "pdfmodel.h"

struct MimeTypeMapping
{
    const char* mimeType;
    LoadHandler::FileType fileType;
    const char* suffix;
    const char* alternativeSuffix;
};

const MimeTypeMapping mimeTypeMappings[] =
{
    { "application/pdf", LoadHandler::PDF, "pdf", 0 },
    { "application/postscript", LoadHandler::PS, "ps", "eps" },
    { "image/vnd.djvu", LoadHandler::DjVu, "djvu", "djv" },
    { "application/x-gzip", LoadHandler::GZip, "gz", 0 },
    { "application/x-bzip2", LoadHandler::BZip2, "bz2", 0 },
    { "application/x-xz", LoadHandler::XZ, "xz", 0 },
    { "application/epub+zip", LoadHandler::EPUB, "epub", 0 },
    { "application/x-fictionbook+xml", LoadHandler::FB2, "fb2", 0 },
    { "application/x-zip-compressed-fb2", LoadHandler::FB2, "fb2", 0 },
    { "application/zip", LoadHandler::ZIP, "zip", 0 }
};

const MimeTypeMapping* const endOfMimeTypeMappings = mimeTypeMappings + sizeof(mimeTypeMappings) / sizeof(mimeTypeMappings[0]);

void matchArchiveAndImageType(const QString& filePath, LoadHandler::FileType& fileType)
{
    if(fileType == LoadHandler::ZIP)
    {
        const QString suffix = QFileInfo(filePath).suffix().toLower();

        if (suffix == "cbz")
        {
            fileType = LoadHandler::CBZ;
        }
        else if (suffix == "xps" || suffix == "oxps")
        {
            fileType = LoadHandler::XPS;
        }
        else
        {
            fileType = LoadHandler::Unknown;
        }
    }

    if(fileType == LoadHandler::Unknown && !QImageReader::imageFormat(filePath).isEmpty())
    {
        fileType = LoadHandler::Image;
    }
}

LoadHandler::FileType matchFileType(const QString& filePath)
{
    LoadHandler::FileType fileType = LoadHandler::Unknown;

    const QMimeType mimeType = QMimeDatabase().mimeTypeForFile(filePath, QMimeDatabase::MatchContent);

    for(const MimeTypeMapping* mapping = mimeTypeMappings; mapping != endOfMimeTypeMappings; ++mapping)
    {
        if(mimeType.inherits(mapping->mimeType))
        {
            fileType = mapping->fileType;
            break;
        }
    }

    matchArchiveAndImageType(filePath, fileType);

    if(fileType == LoadHandler::Unknown)
    {
        qDebug() << "Unknown MIME type:" << mimeType.name();
    }
}

LoadHandler* LoadHandler::s_instance = 0;
LoadHandler *LoadHandler::instance()
{
    if(s_instance == 0)
    {
        s_instance = new LoadHandler;
    }
    return s_instance;
}

LoadHandler::LoadHandler():
    m_plugins()
{

}

LoadHandler::~LoadHandler()
{
    s_instance = 0;
}

ModelDocument *LoadHandler::loadDocument(const QString &filePath)
{
    //create diff model by FileType to support some format
    FileType fileType = matchFileType(filePath);
    QString adjustedFilePath = filePath;
    ModelDocument* mdoc=nullptr;
    if(fileType==PDF)
    {
        //begin>>2019-8-30 kyz test
        PdfPlugin* plug=new PdfPlugin;
        mdoc=plug->loadDocument(filePath);
        delete  plug;
        //end<<
    //mdoc=m_plugins.value(fileType)->loadDocument(filePath);
    }

    return  mdoc;

}



#include "docummentdjvu.h"
#include "pagedjvu.h"
#include "docview/publicfunc.h"
#include <DScrollBar>
#include <QImage>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDebug>

void waitForMessageTag(ddjvu_context_t *context, ddjvu_message_tag_t tag)
{
    ddjvu_message_wait(context);

    while (true) {
        ddjvu_message_t *message = ddjvu_message_peek(context);

        if (message != 0) {
            if (message->m_any.tag == tag) {
                break;
            }

            ddjvu_message_pop(context);
        } else {
            break;
        }
    }
}

void DocummentDJVUPrivate::loadDocumment(QString filepath)
{
    Q_Q(DocummentDJVU);
    m_context = ddjvu_context_create("deepin_reader");

    if (m_context == 0) {
        emit signal_docummentLoaded(false);
        return;
    }

#if DDJVUAPI_VERSION >= 19

    document = ddjvu_document_create_by_filename_utf8(m_context, filepath.toUtf8(), FALSE);

#else

    document = ddjvu_document_create_by_filename(context, QFile::encodeName(filepath), FALSE);

#endif // DDJVUAPI_VERSION

    if (document == 0) {
        ddjvu_context_release(m_context);
        emit signal_docummentLoaded(false);
        return;
    }

    waitForMessageTag(m_context, DDJVU_DOCINFO);

    if (ddjvu_document_decoding_error(document)) {
        ddjvu_document_release(document);
        ddjvu_context_release(m_context);
        emit signal_docummentLoaded(false);
        return;
    }
    unsigned int mask[] = {0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000};

    m_format = ddjvu_format_create(DDJVU_FORMAT_RGBMASK32, 4, mask);
    ddjvu_format_set_row_order(m_format, 1);
    ddjvu_format_set_y_direction(m_format, 1);
    setBasicInfo(filepath);
    donotneedreloaddoc = true;
    m_pages.clear();
    qDebug() << "djvu numPages :" << ddjvu_document_get_pagenum(document);
    for (int i = 0; i < ddjvu_document_get_pagenum(document); i++) {
        PageDJVU *page = new PageDJVU(q);
        page->setPage(i);
        m_pages.append((PageBase *)page);
    }
    setBasicInfo(filepath);

    emit signal_docummentLoaded(true);
}


void DocummentDJVUPrivate::setBasicInfo(const QString &filepath)
{
    QFileInfo info(filepath);
    m_fileinfo.size = info.size();
    m_fileinfo.CreateTime = info.birthTime();
    m_fileinfo.ChangeTime = info.lastModified();
    m_fileinfo.strAuther = info.owner();
    m_fileinfo.strFilepath = info.filePath();
    if (document) {
        int major, minor;
        //document->getPdfVersion(&major, &minor);
        m_fileinfo.strFormat.arg("PDF v.%1.%2", major, minor);
//        m_fileinfo.strKeyword = document->info(QStringLiteral("Keywords"));
//        m_fileinfo.strTheme = document->title();
//        m_fileinfo.strProducter = document->producer();
//        m_fileinfo.strCreater = document->creator();
//        m_fileinfo.bsafe = document->isEncrypted();
//        m_fileinfo.iWidth = document->page(0)->pageSize().width();
//        m_fileinfo.iHeight = document->page(0)->pageSize().height();
//        m_fileinfo.iNumpages = document->numPages();

        /*---------djyu code--------*/
        for (int index = 0, count = ddjvu_document_get_filenum(document); index < count; ++index) {
            ddjvu_fileinfo_t fileinfo;

            if (ddjvu_document_get_fileinfo(document, index, &fileinfo) != DDJVU_JOB_OK || fileinfo.type != 'P') {
                continue;
            }

            const QString id = QString::fromUtf8(fileinfo.id);
            const QString name = QString::fromUtf8(fileinfo.name);
            const QString title = QString::fromUtf8(fileinfo.title);

            m_pageByName[id] = m_pageByName[name] = m_pageByName[title] = fileinfo.pageno + 1;

            if (!title.endsWith(".djvu", Qt::CaseInsensitive) && !title.endsWith(".djv", Qt::CaseInsensitive)) {
                m_titleByIndex[fileinfo.pageno] = title;
            }
        }

        m_pageByName.squeeze();
        m_titleByIndex.squeeze();
    }
}


DocummentDJVU::DocummentDJVU(DWidget *parent):
    DocummentBase(new DocummentDJVUPrivate(this), parent)
{
}

DocummentDJVU::~DocummentDJVU()
{
}


bool DocummentDJVU::loadDocumment(QString filepath)
{

    emit signal_loadDocumment(filepath);
    return true;
}

bool DocummentDJVU::bDocummentExist()
{
    Q_D(DocummentDJVU);
    if (!d->document) {
        return false;
    }
    return true;
}

bool DocummentDJVU::getImage(int pagenum, QImage &image, double width, double height)
{
    Q_D(DocummentDJVU);
    return d->m_pages.at(pagenum)->getInterFace()->getImage(image, width, height);
}

void DocummentDJVU::docBasicInfo(stFileInfo &info)
{
    Q_D(DocummentDJVU);
    info = d->m_fileinfo;
}

//bool DocummentDJVU::annotationClicked(const QPoint &pos, QString &strtext)
//{
//    QPoint pt(pos);
//    int ipage = pointInWhichPage(pt);
//    if (ipage < 0) return  false;
//    return static_cast<PageDJVU>(m_pages.at(ipage)).annotationClicked(pt, strtext);
//}

//void DocummentDJVU::title(QString &title)
//{
////    title = document->title();
//}

ddjvu_document_t *DocummentDJVU::getDocument()
{
    Q_D(DocummentDJVU);
    return d->document;
}
ddjvu_context_t *DocummentDJVU::getContext()
{
    Q_D(DocummentDJVU);
    return d->m_context;
}
ddjvu_format_t *DocummentDJVU::getFormat()
{
    Q_D(DocummentDJVU);
    return d->m_format;
}
QHash< QString, int > DocummentDJVU::getPageByName()
{
    Q_D(DocummentDJVU);
    return d->m_pageByName;
}

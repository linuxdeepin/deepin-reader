#include "docummentps.h"
#include "pageps.h"
#include "docview/publicfunc.h"
//#include "../docummentbase.cpp"
#include <QImage>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDebug>

static const int graphicsAntialiasBits = 4;
static const int textAntialiasBits = 2;


class DocummentPSPrivate: public DocummentBasePrivate
{
//    Q_OBJECT
public:
    DocummentPSPrivate(DocummentPS *parent): DocummentBasePrivate(parent)
    {
        document = nullptr;
        m_renderContext = nullptr;
        m_settings = new QSettings("docummentps", "deepin_reader", this);
    }

    ~DocummentPSPrivate()
    {
        if (nullptr != m_renderContext) {
            spectre_render_context_free(m_renderContext);
            m_renderContext = nullptr;
        }

        if (nullptr != document) {
            spectre_document_free(document);
            document = nullptr;
        }
    }

    SpectreDocument *document;
    SpectreRenderContext *m_renderContext;
//    stFileInfo m_fileinfo;
    QSettings *m_settings;
    Q_DECLARE_PUBLIC(DocummentPS)
protected slots:
    void loadDocumment(QString filepath) override;
private:
    void setBasicInfo(const QString &filepath);
};

void DocummentPSPrivate::loadDocumment(QString filepath)
{
    Q_Q(DocummentPS);

    document = spectre_document_new();

    spectre_document_load(document, QFile::encodeName(filepath));

    if (spectre_document_status(document) != SPECTRE_STATUS_SUCCESS) {
        spectre_document_free(document);
        emit signal_docummentLoaded(false);
        return;
    }

    m_renderContext = spectre_render_context_new();

    spectre_render_context_set_antialias_bits(m_renderContext,
                                              m_settings->value("graphicsAntialiasBits", graphicsAntialiasBits).toInt(),
                                              m_settings->value("textAntialiasBits", textAntialiasBits).toInt());
    donotneedreloaddoc = true;
    m_pages.clear();
    qDebug() << "djvu numPages :" << spectre_document_get_n_pages(document);
    for (int i = 0; i < spectre_document_get_n_pages(document); i++) {
        PagePS *page = new PagePS(q);
        page->setPage(spectre_document_get_page(document, i), m_renderContext, i);
        m_pages.append((PageBase *)page);
    }
    if (m_pages.size() > 0) {
        m_imagewidth = m_pages.at(0)->getOriginalImageWidth();
        m_imageheight = m_pages.at(0)->getOriginalImageHeight();
    }
    setBasicInfo(filepath);

    emit signal_docummentLoaded(true);
}


void DocummentPSPrivate::setBasicInfo(const QString &filepath)
{
    QFileInfo info(filepath);
    m_fileinfo->size = info.size();
    m_fileinfo->CreateTime = info.birthTime();
    m_fileinfo->ChangeTime = info.lastModified();
    m_fileinfo->strAuther = info.owner();
    m_fileinfo->strFilepath = info.filePath();
    if (document) {
        int major = 0, minor = 0;
        m_fileinfo->strFormat.arg("PS v.%1.%2", major, minor);
    }
}



DocummentPS::DocummentPS(DWidget *parent):
    DocummentBase(new DocummentPSPrivate(this), parent)
{
}

DocummentPS::~DocummentPS()
{
}

bool DocummentPS::loadDocumment(QString filepath)
{
    emit signal_loadDocumment(filepath);
    return true;
}

bool DocummentPS::bDocummentExist()
{
    Q_D(DocummentPS);
    if (!d->document) {
        return false;
    }
    return true;
}

//bool DocummentPS::getImage(int pagenum, QImage &image, double width, double height)
//{
//    Q_D(DocummentPS);
//    qreal pixelratiof = d->m_pages.at(pagenum)->devicePixelRatioF();
//    if (!d->m_pages.at(pagenum)->getImage(image, width * pixelratiof, height * pixelratiof)) {
//        return false;
//    }
//    image.setDevicePixelRatio(d->m_pages.at(pagenum)->devicePixelRatioF());
//    return true;
//}

//void DocummentPS::docBasicInfo(stFileInfo &info)
//{
//    Q_D(DocummentPS);
//    info = d->m_fileinfo;
//}

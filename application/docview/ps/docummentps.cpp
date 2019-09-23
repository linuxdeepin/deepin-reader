#include "docummentps.h"
#include "pageps.h"
#include "docview/publicfunc.h"
#include <QImage>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDebug>

static const int graphicsAntialiasBits = 4;
static const int textAntialiasBits = 2;
DocummentPS::DocummentPS(DWidget *parent): DocummentBase(parent),
    document(nullptr),
    m_renderContext(nullptr)
{
    m_settings = new QSettings("docummentps", "deepin_reader", this);
}

DocummentPS::~DocummentPS()
{
    spectre_render_context_free(m_renderContext);
    m_renderContext = 0;

    spectre_document_free(document);
    document = 0;
}

bool DocummentPS::openFile(QString filepath)
{


    document = spectre_document_new();

    spectre_document_load(document, QFile::encodeName(filepath));

    if (spectre_document_status(document) != SPECTRE_STATUS_SUCCESS) {
        spectre_document_free(document);

        return 0;
    }

    m_renderContext = spectre_render_context_new();

    spectre_render_context_set_antialias_bits(m_renderContext,
                                              m_settings->value("graphicsAntialiasBits", graphicsAntialiasBits).toInt(),
                                              m_settings->value("textAntialiasBits", textAntialiasBits).toInt());
    setBasicInfo(filepath);
    donotneedreloaddoc = true;
    m_pages.clear();
    qDebug() << "djvu numPages :" << spectre_document_get_n_pages(document);
    for (int i = 0; i < spectre_document_get_n_pages(document); i++) {
        DWidget *qwidget = new DWidget(this);
        QHBoxLayout *qhblayout = new QHBoxLayout(qwidget);
        qhblayout->setAlignment(qwidget, Qt::AlignCenter);
        qwidget->setLayout(qhblayout);
        m_vboxLayout.addWidget(qwidget);
        //        m_vboxLayout.addWidget(m_pages.at(i));
        m_vboxLayout.setAlignment(&m_widget, Qt::AlignCenter);
        qwidget->setMouseTracking(true);
        m_widgets.append(qwidget);

        PagePS *page = new PagePS(this);
        page->setPage(spectre_document_get_page(document, i), m_renderContext);
        m_pages.append((PageBase *)page);
    }

    for (int i = 0; i < m_pages.size(); i++) {
        m_pages.at(i)->setScaleAndRotate(m_scale, m_rotate);
    }
    setViewModeAndShow(m_viewmode);
    donotneedreloaddoc = false;
    if (m_threadloaddoc.isRunning())
        m_threadloaddoc.setRestart();
    else
        m_threadloaddoc.start();
    if (m_threadloadwords.isRunning())
        m_threadloadwords.setRestart();
    else
        m_threadloadwords.start();

    return true;
}

//bool DocummentPS::loadPages()
//{
//    if (!document && m_pages.size() == spectre_document_get_n_pages(document))
//        return false;
//    qDebug() << "loadPages";
//    //    for (int i = 0; i < m_pages.size(); i++) {
//    int startnum = m_currentpageno - 3;
//    if (startnum < 0) {
//        startnum = 0;
//    }
//    int endnum = startnum + 7;
//    if (endnum > m_pages.size()) {
//        endnum = m_pages.size();
//    }
//    for (int i = startnum; i < endnum; i++) {
//        if (QThread::currentThread()->isInterruptionRequested()) {
//            break;
//        }
//        m_pages.at(i)->showImage(m_scale, m_rotate);
//    }
//    return true;
//}

bool DocummentPS::loadWords()
{
    if (!document && m_pages.size() == spectre_document_get_n_pages(document))
        return false;
    qDebug() << "loadWords";
    for (int i = 0; i < m_pages.size(); i++) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        PagePS *pps = (PagePS *)m_pages.at(i);
        pps->loadWords();
        pps->loadLinks();
    }
    return true;
}

void DocummentPS::setBasicInfo(const QString &filepath)
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

//        /*---------djyu code--------*/
//        for (int index = 0, count = spectre_document_get_n_pages(document); index < count; ++index) {
//            ddjvu_fileinfo_t fileinfo;

//            if (ddjvu_document_get_fileinfo(document, index, &fileinfo) != DDJVU_JOB_OK || fileinfo.type != 'P') {
//                continue;
//            }

//            const QString id = QString::fromUtf8(fileinfo.id);
//            const QString name = QString::fromUtf8(fileinfo.name);
//            const QString title = QString::fromUtf8(fileinfo.title);

//            m_pageByName[id] = m_pageByName[name] = m_pageByName[title] = fileinfo.pageno + 1;

//            if (!title.endsWith(".djvu", Qt::CaseInsensitive) && !title.endsWith(".djv", Qt::CaseInsensitive)) {
//                m_titleByIndex[fileinfo.pageno] = title;
//            }
//        }

//        m_pageByName.squeeze();
//        m_titleByIndex.squeeze();
    }
}

bool DocummentPS::bDocummentExist()
{
    if (!document) {
        return false;
    }
    return true;
}

bool DocummentPS::getImage(int pagenum, QImage &image, double width, double height)
{
    return m_pages.at(pagenum)->getImage(image, width, height);
}

void DocummentPS::docBasicInfo(stFileInfo &info)
{
    info = m_fileinfo;
}

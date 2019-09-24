#include "docummentxps.h"
#include "pagexps.h"
#include "docview/publicfunc.h"
#include <QImage>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDebug>

static const int graphicsAntialiasBits = 4;
static const int textAntialiasBits = 2;

DocummentXPS::DocummentXPS(DWidget *parent): DocummentBase(parent),
    m_xpsFile(nullptr)
{
    m_xpsFile = new XpsFile();
}

DocummentXPS::~DocummentXPS()
{
    delete m_xpsFile;
}

bool DocummentXPS::openFile(QString filepath)
{

    m_xpsFile->loadDocument( filepath );

    int pagesVectorOffset = 0;

    qDebug() << "numDocuments:" << m_xpsFile->numDocuments();
    if (m_xpsFile->numDocuments() < 1)
        return false;
    setBasicInfo(filepath);
    donotneedreloaddoc = true;
    m_pages.clear();
    for (int docNum = 0; docNum < m_xpsFile->numDocuments(); ++docNum ) {
        XpsDocument *document = m_xpsFile->document( docNum );
        qDebug() << "xps numPages :" << document->numPages();
        for (int i = 0; i < document->numPages(); i++) {
            DWidget *qwidget = new DWidget(this);
            QHBoxLayout *qhblayout = new QHBoxLayout(qwidget);
            qhblayout->setAlignment(qwidget, Qt::AlignCenter);
            qwidget->setLayout(qhblayout);
            m_vboxLayout.addWidget(qwidget);
            //        m_vboxLayout.addWidget(m_pages.at(i));
            m_vboxLayout.setAlignment(&m_widget, Qt::AlignCenter);
            qwidget->setMouseTracking(true);
            m_widgets.append(qwidget);

            PageXPS *page = new PageXPS(this);
            page->setPage(document->page(i));
            m_pages.append((PageBase *)page);
        }
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

//bool DocummentXPS::loadPages()
//{
//    if (!m_xpsFile)
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

//bool DocummentXPS::loadWords()
//{
//    if (!m_xpsFile)
//        return false;
//    qDebug() << "loadWords";
//    for (int i = 0; i < m_pages.size(); i++) {
//        PageXPS *pps = (PageXPS *)m_pages.at(i);
//        pps->loadWords();
//        pps->loadLinks();
//    }
//    return true;
//}

void DocummentXPS::setBasicInfo(const QString &filepath)
{
    QFileInfo info(filepath);
    m_fileinfo.size = info.size();
    m_fileinfo.CreateTime = info.birthTime();
    m_fileinfo.ChangeTime = info.lastModified();
    m_fileinfo.strAuther = info.owner();
    m_fileinfo.strFilepath = info.filePath();
    if (m_xpsFile) {
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

bool DocummentXPS::bDocummentExist()
{
    if (!m_xpsFile) {
        return false;
    }
    return true;
}

bool DocummentXPS::getImage(int pagenum, QImage &image, double width, double height)
{
    return m_pages.at(pagenum)->getImage(image, width, height);
}

void DocummentXPS::docBasicInfo(stFileInfo &info)
{
    info = m_fileinfo;
}

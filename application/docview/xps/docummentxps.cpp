#include "docummentxps.h"
#include "pagexps.h"
#include "docview/publicfunc.h"
//#include "../docummentbase.cpp"
#include <QImage>
#include <QTemporaryFile>
#include <QFileInfo>
#include <QDebug>


class DocummentXPSPrivate: public DocummentBasePrivate
{
//    Q_OBJECT
public:
    DocummentXPSPrivate(DocummentXPS *parent): DocummentBasePrivate(parent)
    {
        m_xpsFile = new XpsFile();
    }

    ~DocummentXPSPrivate() override
    {
        delete m_xpsFile;
    }

    stFileInfo m_fileinfo;
    XpsFile *m_xpsFile;
    Q_DECLARE_PUBLIC(DocummentXPS)
protected slots:
    void loadDocumment(QString filepath) override;
private:
    void setBasicInfo(const QString &filepath);
};

void DocummentXPSPrivate::loadDocumment(QString filepath)
{
    Q_Q(DocummentXPS);

    m_xpsFile->loadDocument( filepath );


    qDebug() << "numDocuments:" << m_xpsFile->numDocuments();
    if (m_xpsFile->numDocuments() < 1) {
        emit signal_docummentLoaded(false);
        return;
    }
    setBasicInfo(filepath);
    donotneedreloaddoc = true;
    m_pages.clear();
    for (int docNum = 0; docNum < m_xpsFile->numDocuments(); ++docNum ) {
        XpsDocument *document = m_xpsFile->document( docNum );
        qDebug() << "xps numPages :" << document->numPages();
        for (int i = 0; i < document->numPages(); i++) {
            PageXPS *page = new PageXPS(q);
            page->setPage(document->page(i), i);
            m_pages.append((PageBase *)page);
        }
    }

    setBasicInfo(filepath);

    emit signal_docummentLoaded(true);
}


void DocummentXPSPrivate::setBasicInfo(const QString &filepath)
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

    }
}

DocummentXPS::DocummentXPS(DWidget *parent):
    DocummentBase(new DocummentXPSPrivate(this), parent)
{
}

DocummentXPS::~DocummentXPS()
{
}

bool DocummentXPS::loadDocumment(QString filepath)
{
    emit signal_loadDocumment(filepath);
    return true;
}

bool DocummentXPS::bDocummentExist()
{
    Q_D(DocummentXPS);
    if (!d->m_xpsFile) {
        return false;
    }
    return true;
}

bool DocummentXPS::getImage(int pagenum, QImage &image, double width, double height)
{
    Q_D(DocummentXPS);
    return d->m_pages.at(pagenum)->getInterFace()->getImage(image, width, height);
}

void DocummentXPS::docBasicInfo(stFileInfo &info)
{
    Q_D(DocummentXPS);
    info = d->m_fileinfo;
}

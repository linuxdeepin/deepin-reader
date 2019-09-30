#include "docummentproxy.h"
#include "docummentfactory.h"
#include "publicfunc.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

DocummentProxy *DocummentProxy::s_pDocProxy = nullptr;

ThreadWaitLoadWordsEnd::ThreadWaitLoadWordsEnd()
{
    //    connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
    m_doc = nullptr;
    restart = false;
}

void ThreadWaitLoadWordsEnd::setRestart()
{
    restart = true;
}

void ThreadWaitLoadWordsEnd::setDoc(DocummentBase *doc)
{
    m_doc = doc;
}

void ThreadWaitLoadWordsEnd::run()
{
    if (!m_doc)
        return;
    restart = true;
    while (restart) {
        restart = false;
        while (m_doc->isWordsBeLoad())
            QThread::msleep(30);

        emit startOpenFile();
    }
}


DocummentProxy::DocummentProxy(QObject *parent)
    : QObject(parent),
      m_type(DocType_NULL),
      m_path(""),
      m_documment(nullptr),
      bclosefile(false),
      bcloseing(false)
{
    qwfather = (DWidget *)parent;
    connect(&threadwaitloadwordsend, SIGNAL(startOpenFile()), this, SLOT(startOpenFile()));
}

DocummentProxy *DocummentProxy::instance(QObject *parent)
{
    if (nullptr != parent && nullptr == s_pDocProxy) {
        s_pDocProxy = new DocummentProxy(parent);
    }
    return  s_pDocProxy;
}

bool DocummentProxy::openFile(DocType_EM type, QString filepath)
{
    m_type = type;
    m_path = filepath;
    if (nullptr != m_documment) {
        bcloseing = true;
        m_documment->stopLoadPageThread();
        if (m_documment->isWordsBeLoad()) {
            qDebug() << "threadwaitloadwordsend true";
            bclosefile = false;
            if (!threadwaitloadwordsend.isRunning()) {
                threadwaitloadwordsend.setDoc(m_documment);
                threadwaitloadwordsend.start();
            }
            return true;
        }
    }
    return startOpenFile();
}

bool DocummentProxy::startOpenFile()
{
    if (nullptr != m_documment) {
        delete m_documment;
        m_documment = nullptr;
    }
    bcloseing = false;
    if (bclosefile) {
        return false;
    }
    m_documment = DocummentFactory::creatDocumment(m_type, qwfather);
    connect(m_documment, SIGNAL(signal_pageChange(int)), this, SLOT(slot_pageChange(int)));
    connect(this, SIGNAL(signal_pageJump(int)), m_documment, SLOT(pageJump(int)));
    connect(m_documment, SIGNAL(signal_searchRes(stSearchRes)),this, SIGNAL(signal_searchRes(stSearchRes)));
    connect(this, SIGNAL(signal_mouseSelectText(QPoint, QPoint)), m_documment, SLOT(mouseSelectText(QPoint, QPoint)));
    connect(this, SIGNAL(signal_scaleAndShow(double, RotateType_EM)), m_documment, SLOT(scaleAndShow(double, RotateType_EM)));
    connect(this, SIGNAL(signal_setViewModeAndShow(ViewMode_EM)), m_documment, SLOT(setViewModeAndShow(ViewMode_EM)));

    return m_documment->openFile(m_path);
}

bool DocummentProxy::setSelectTextStyle(QColor paintercolor, QColor pencolor, int penwidth)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->setSelectTextStyle(paintercolor, pencolor, penwidth);
}

bool DocummentProxy::mouseSelectText(QPoint start, QPoint stop)
{
    if (!m_documment || bcloseing)
        return false;
    return emit signal_mouseSelectText(start, stop);
//    return m_documment->mouseSelectText(start, stop);
}

void DocummentProxy::mouseSelectTextClear()
{
    m_documment->mouseSelectTextClear();
}

bool DocummentProxy::mouseBeOverText(QPoint point)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->mouseBeOverText(point);
}

void DocummentProxy::scaleRotateAndShow(double scale, RotateType_EM rotate)
{
    if (!m_documment || bcloseing)
        return;
    mouseSelectTextClear();
//    m_documment->scaleAndShow(scale, rotate);
    emit signal_scaleAndShow(scale, rotate);
}

QPoint DocummentProxy::global2RelativePoint(QPoint globalpoint)
{
    if (!m_documment || bcloseing)
        return QPoint();
    return m_documment->global2RelativePoint(globalpoint);
}

bool DocummentProxy::getImage(int pagenum, QImage &image, double width, double height)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->getImage(pagenum, image, width, height);
}

int DocummentProxy::getPageSNum()
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->getPageSNum();
}

bool DocummentProxy::setViewModeAndShow(ViewMode_EM viewmode)
{
    if (!m_documment || bcloseing)
        return false;
    mouseSelectTextClear();
//    return  m_documment->setViewModeAndShow(viewmode);
    return emit signal_setViewModeAndShow(viewmode);
}

bool DocummentProxy::showMagnifier(QPoint point)
{
    if (!m_documment || bcloseing)
        return false;
    return  m_documment->showMagnifier(point);
}

bool DocummentProxy::closeMagnifier()
{
    if (!m_documment || bcloseing)
        return false;
    m_documment->magnifierClear();
    return true;
}

bool DocummentProxy::setMagnifierStyle(QColor magnifiercolor, int magnifierradius, int magnifierringwidth, double magnifierscale)
{
    if (!m_documment || bcloseing)
        return false;
    return  m_documment->setMagnifierStyle(magnifiercolor, magnifierradius, magnifierringwidth, magnifierscale);
}
QString DocummentProxy::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    if (!m_documment || bcloseing)
        return QString("");
    return m_documment->addAnnotation(startpos, endpos, color);
}

bool DocummentProxy::save(const QString &filepath, bool withChanges)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->save(filepath, withChanges);
}

bool DocummentProxy::saveas(const QString &filepath, bool withChanges)
{
    if (m_documment && !bcloseing && m_documment->saveas(filepath, withChanges)) {
        return openFile(DocType_PDF, filepath);
    }
    return false;
}

void DocummentProxy::search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color)
{
    if (!m_documment || bcloseing)
        return ;
    m_documment->search(strtext, color);
}

void DocummentProxy::clearsearch()
{
    if (!m_documment || bcloseing)
        return ;
    m_documment->clearSearch();
}

int DocummentProxy::currentPageNo()
{
    if (!m_documment || bcloseing)
        return -1;
    return m_documment->currentPageNo();
}

bool DocummentProxy::pageJump(int pagenum)
{
    if (!m_documment || bcloseing)
        return false;

    return emit signal_pageJump(pagenum);;
    //    return m_documment->pageJump(pagenum);
}

void DocummentProxy::docBasicInfo(stFileInfo &info)
{
    if (!m_documment || bcloseing)
        return ;
    m_documment->docBasicInfo(info);
}

QString DocummentProxy::removeAnnotation(const QPoint &startpos)
{
    if (!m_documment || bcloseing)
        return "";
    return m_documment->removeAnnotation(startpos);
}

void DocummentProxy::removeAnnotation(const QString &struuid)
{
    if (!m_documment || bcloseing)
        return ;
    m_documment->removeAnnotation(struuid);
}

void DocummentProxy::slot_pageChange(int pageno)
{
    emit signal_pageChange(pageno);
}

bool DocummentProxy::pageMove(double mvx, double mvy)
{
    if (!m_documment || bcloseing)
        return false;
    m_documment->pageMove(mvx, mvy);
    return true;
}

void DocummentProxy::title(QString &title)
{
    if (!m_documment || bcloseing)
        return;
    m_documment->title(title);
}

Page::Link *DocummentProxy::mouseBeOverLink(QPoint point)
{
    if (!m_documment || bcloseing)
        return nullptr;
    return m_documment->mouseBeOverLink(point);
}

bool DocummentProxy::getSelectTextString(QString &st)
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->getSelectTextString(st);
}


bool DocummentProxy::showSlideModel()
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->showSlideModel();
}
bool DocummentProxy::exitSlideModel()
{
    if (!m_documment || bcloseing)
        return false;
    return m_documment->exitSlideModel();
}

void DocummentProxy::findNext()
{
    if (m_documment || bcloseing) {
        m_documment->findNext();
    }
}

void DocummentProxy::findPrev()
{
    if (m_documment || bcloseing) {
        m_documment->findPrev();
    }
}

void DocummentProxy::setAnnotationText(int ipage, const QString &struuid, const QString &strtext)
{
    if (m_documment || bcloseing) {
        m_documment->setAnnotationText(ipage, struuid, strtext);
    }
}

void DocummentProxy::getAnnotationText(const QString &struuid, QString &strtext, int ipage)
{
    if (m_documment || bcloseing) {
        m_documment->getAnnotationText(struuid, strtext, ipage);
    }
}

double DocummentProxy::adaptWidthAndShow(double width)
{
    if (!m_documment || bcloseing)
        return -1;
    qDebug() << "adaptWidthAndShow width:" << width;
    return m_documment->adaptWidthAndShow(width);
}

double DocummentProxy::adaptHeightAndShow(double height)
{
    if (!m_documment || bcloseing)
        return -1;
    qDebug() << "adaptHeightAndShow height:" << height;
    return m_documment->adaptHeightAndShow(height);
}

bool DocummentProxy::annotationClicked(const QPoint &pos, QString &strtext)
{
    if(m_documment)
    {
        return m_documment->annotationClicked(pos,strtext);
    }
    return  false;
}

bool DocummentProxy::closeFile()
{
    if (!m_documment)
        return false;
    if (nullptr != m_documment) {
        bcloseing = true;
        m_documment->stopLoadPageThread();
        if (m_documment->isWordsBeLoad()) {
            qDebug() << "threadwaitloadwordsend true";
            bclosefile = true;
            if (!threadwaitloadwordsend.isRunning()) {
                threadwaitloadwordsend.setDoc(m_documment);
                threadwaitloadwordsend.start();
            }
            return true;
        }
    }
    return true;
}

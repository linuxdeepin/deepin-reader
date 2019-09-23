#include "docummentproxy.h"
#include "docummentfactory.h"
#include "publicfunc.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

DocummentProxy *DocummentProxy::s_pDocProxy = nullptr;
DocummentProxy::DocummentProxy(QObject *parent)
    : QObject(parent),
      m_type(DocType_NULL),
      m_path(""),
      m_documment(nullptr)
{
    qwfather = (DWidget *)parent;
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
    if (nullptr != m_documment) {
        delete m_documment;
        m_documment = nullptr;
    }
    m_type = type;
    m_documment = DocummentFactory::creatDocumment(type, qwfather);
    m_path = filepath;
    connect(m_documment, SIGNAL(signal_pageChange(int)), this, SLOT(slot_pageChange(int)));
    return m_documment->openFile(filepath);
}

bool DocummentProxy::setSelectTextStyle(QColor paintercolor, QColor pencolor, int penwidth)
{
    if (!m_documment)
        return false;
    return m_documment->setSelectTextStyle(paintercolor, pencolor, penwidth);
}

bool DocummentProxy::mouseSelectText(QPoint start, QPoint stop)
{
    if (!m_documment)
        return false;
    return m_documment->mouseSelectText(start, stop);
}

void DocummentProxy::mouseSelectTextClear()
{
    m_documment->mouseSelectTextClear();
}

bool DocummentProxy::mouseBeOverText(QPoint point)
{
    if (!m_documment)
        return false;
    return m_documment->mouseBeOverText(point);
}

void DocummentProxy::scaleRotateAndShow(double scale, RotateType_EM rotate)
{
    if (!m_documment)
        return;
    m_documment->scaleAndShow(scale, rotate);

}

QPoint DocummentProxy::global2RelativePoint(QPoint globalpoint)
{
    if (!m_documment)
        return QPoint();
    return m_documment->global2RelativePoint(globalpoint);
}

bool DocummentProxy::getImage(int pagenum, QImage &image, double width, double height)
{
    if (!m_documment)
        return false;
    return m_documment->getImage(pagenum, image, width, height);
}

int DocummentProxy::getPageSNum()
{
    if (!m_documment)
        return false;
    return m_documment->getPageSNum();
}

bool DocummentProxy::setViewModeAndShow(ViewMode_EM viewmode)
{
    if (!m_documment)
        return false;
    return  m_documment->setViewModeAndShow(viewmode);
}

bool DocummentProxy::showMagnifier(QPoint point)
{
    if (!m_documment)
        return false;
    return  m_documment->showMagnifier(point);
}

bool DocummentProxy::closeMagnifier()
{
    if (!m_documment)
        return false;
    m_documment->magnifierClear();
    return true;
}

bool DocummentProxy::setMagnifierStyle(QColor magnifiercolor, int magnifierradius, int magnifierringwidth, double magnifierscale)
{
    if (!m_documment)
        return false;
    return  m_documment->setMagnifierStyle(magnifiercolor, magnifierradius, magnifierringwidth, magnifierscale);
}
QString DocummentProxy::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    if (!m_documment)
        return QString("");
    return m_documment->addAnnotation(startpos, endpos, color);
}

bool DocummentProxy::save(const QString &filepath, bool withChanges)
{
    if (!m_documment)
        return false;
    return m_documment->save(filepath, withChanges);
}

void DocummentProxy::search(const QString &strtext, QMap<int, stSearchRes> &resmap, const QColor &color)
{
    if (!m_documment)
        return ;
    m_documment->search(strtext, resmap, color);
}

void DocummentProxy::clearsearch()
{
    if (!m_documment)
        return ;
    m_documment->clearSearch();
}

int DocummentProxy::currentPageNo()
{
    if (!m_documment)
        return -1;
    return m_documment->currentPageNo();
}

bool DocummentProxy::pageJump(int pagenum)
{
    if (!m_documment)
        return false;
    return m_documment->pageJump(pagenum);
}

void DocummentProxy::docBasicInfo(stFileInfo &info)
{
    if (!m_documment)
        return ;
    m_documment->docBasicInfo(info);
}

QString DocummentProxy::removeAnnotation(const QPoint &startpos)
{
    if (!m_documment)
        return "";
    return m_documment->removeAnnotation(startpos);
}

void DocummentProxy::removeAnnotation(const QString &struuid)
{
    if (!m_documment)
        return ;
    m_documment->removeAnnotation(struuid);
}

void DocummentProxy::slot_pageChange(int pageno)
{
    emit signal_pageChange(pageno);
}

bool DocummentProxy::pageMove(double mvx, double mvy)
{
    if (!m_documment)
        return false;
    m_documment->pageMove(mvx, mvy);
    return true;
}

void DocummentProxy::title(QString &title)
{
    if (!m_documment)
        return;
    m_documment->title(title);
}

Page::Link *DocummentProxy::mouseBeOverLink(QPoint point)
{
    if (!m_documment)
        return nullptr;
    return m_documment->mouseBeOverLink(point);
}

bool DocummentProxy::getSelectTextString(QString &st)
{
    if (!m_documment)
        return false;
    return m_documment->getSelectTextString(st);
}


bool DocummentProxy::showSlideModel()
{
    if (!m_documment)
        return false;
    return m_documment->showSlideModel();
}
bool DocummentProxy::exitSlideModel()
{
    if (!m_documment)
        return false;
    return m_documment->exitSlideModel();
}

void DocummentProxy::findNext()
{
    if (m_documment) {
        m_documment->findNext();
    }
}

void DocummentProxy::findPrev()
{
    if (m_documment) {
        m_documment->findPrev();
    }
}

void DocummentProxy::setAnnotationText(int ipage, const QString &struuid, const QString &strtext)
{
    if (m_documment) {
        m_documment->setAnnotationText(ipage, struuid, strtext);
    }
}

void DocummentProxy::getAnnotationText(const QString &struuid, QString &strtext)
{
    if (m_documment) {
        m_documment->getAnnotationText(struuid, strtext);
    }
}

bool DocummentProxy::adaptWidthAndShow(double width)
{
    if (!m_documment)
        return false;
    return m_documment->adaptWidthAndShow(width);
}

bool DocummentProxy::adaptHeightAndShow(double height)
{
    if (!m_documment)
        return false;
    return m_documment->adaptHeightAndShow(height);
}

bool DocummentProxy::closeFile()
{
    if (!m_documment)
        return false;
    delete m_documment;
    m_documment = nullptr;
    return true;
}

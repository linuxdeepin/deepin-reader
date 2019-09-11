#include "docummentproxy.h"
#include "docummentfactory.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

DocummentProxy* DocummentProxy::s_pDocProxy=nullptr;
DocummentProxy::DocummentProxy(QObject *parent)
    : QObject(parent),
      m_type(DocType_NULL),
      m_path(""),
      m_documment(nullptr)
{
    qwfather = (QWidget *)parent;
}

DocummentProxy *DocummentProxy::instance(QObject *parent)
{
    if(nullptr!=parent&&nullptr==s_pDocProxy)
    {
        s_pDocProxy=new DocummentProxy(parent);
    }
    return  s_pDocProxy;
}

bool DocummentProxy::openFile(DocType_EM type, QString filepath)
{
    m_type = type;
    m_documment = DocummentFactory::creatDocumment(type, qwfather);
    m_path = filepath;
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

bool DocummentProxy::setMagnifierStyle(QColor magnifiercolor, int magnifierradius, int magnifierringwidth, double magnifierscale)
{
    if (!m_documment)
        return false;
    return  m_documment->setMagnifierStyle(magnifiercolor, magnifierradius, magnifierringwidth, magnifierscale);
}
void DocummentProxy::addAnnotation(const QPoint &startpos, const QPoint &endpos, QColor color)
{
    if (!m_documment)
        return ;
    m_documment->addAnnotation(startpos,endpos,color);
}

bool DocummentProxy::save(const QString &filepath, bool withChanges)
{
    if (!m_documment)
        return false;
    m_documment->save(filepath,withChanges);
}

void DocummentProxy::search(const QString &strtext, const QColor &color)
{
    if (!m_documment)
        return ;
    m_documment->search(strtext,color);
}



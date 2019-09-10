#include "docummentproxy.h"
#include "docummentfactory.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

DocummentProxy::DocummentProxy(QObject *parent)
    : QObject(parent),
      m_type(DocType_NULL),
      m_path(""),
      m_documment(nullptr)
{
    qwfather = (QWidget *)parent;
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

bool DocummentProxy::getImage(int pagenum, QImage &image, double width, double height, RotateType_EM rotate)
{
    if (!m_documment)
        return false;
    return m_documment->getImage(pagenum, image, width, height, rotate);
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

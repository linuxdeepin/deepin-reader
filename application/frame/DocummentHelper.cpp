#include "DocummentHelper.h"

DocummentHelper::DocummentHelper(QObject *parent) : QObject(parent)
{
    m_pDocummentProxy = DocummentProxy::instance(this);
}

bool DocummentHelper::openFile(const DocType_EM &type, const QString &filepath)
{
    return m_pDocummentProxy->openFile(type, filepath);
}

bool DocummentHelper::getImage(const int &pagenum, QImage &image, const int &width, const int &height)
{
    return m_pDocummentProxy->getImage(pagenum, image, width, height);
}

QPoint DocummentHelper::global2RelativePoint(const QPoint &globalpoint)
{
    return m_pDocummentProxy->global2RelativePoint(globalpoint);
}

bool DocummentHelper::showMagnifier(const QPoint &globalpoint)
{
    return m_pDocummentProxy->showMagnifier(globalpoint);
}

bool DocummentHelper::closeMagnifier()
{
    return m_pDocummentProxy->closeMagnifier();
}

int DocummentHelper::getPageSNum()
{
    return m_pDocummentProxy->getPageSNum();
}

bool DocummentHelper::pageMove(const double &x, const double &y)
{
    return m_pDocummentProxy->pageMove(x, y);
}

bool DocummentHelper::mouseSelectText(const QPoint &start, const QPoint &end)
{
    return m_pDocummentProxy->mouseSelectText(start, end);
}

bool DocummentHelper::mouseBeOverText(const QPoint &pos)
{
    return m_pDocummentProxy->mouseBeOverText(pos);
}

void DocummentHelper::mouseSelectTextClear()
{
    m_pDocummentProxy->mouseSelectTextClear();
}

bool DocummentHelper::showSlideModel()
{
    return m_pDocummentProxy->showSlideModel();
}

void DocummentHelper::scaleRotateAndShow(const double &scale, const RotateType_EM &rotate)
{
    m_pDocummentProxy->scaleRotateAndShow(scale, rotate);
}

bool DocummentHelper::setViewModeAndShow(const ViewMode_EM &viewmode)
{
    return m_pDocummentProxy->setViewModeAndShow(viewmode);
}

int DocummentHelper::dealWithData(const int &, const QString &)
{
    return 0;
}

void DocummentHelper::sendMsg(const int &, const QString &)
{

}

void DocummentHelper::setObserverName(const QString &)
{
    m_strObserverName = "DocummentHelper";
}

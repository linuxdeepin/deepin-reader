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

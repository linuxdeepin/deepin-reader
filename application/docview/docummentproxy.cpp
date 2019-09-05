#include "docummentproxy.h"
#include "docummentfactory.h"
#include <QLabel>
#include <QVBoxLayout>
#include <QDebug>

DocummentProxy::DocummentProxy(QObject *parent)
    : QObject(parent),
      m_type(DocType_NULL),
      m_path(""),
      m_documment(NULL)
{
    qwfather = (QWidget *)parent;
}

bool DocummentProxy::openFile(DocType_EM type, QString filepath)
{
    DocummentFactory factory;
    m_type = type;
    m_documment = factory.creatDocumment(type, qwfather);
    m_path = filepath;
    QWidget *widget = new QWidget(qwfather);
    widget->setGeometry(0, 0, 300, 300);
    QVBoxLayout *vboxLayout = new QVBoxLayout(qwfather);
    QLabel *label = new QLabel(qwfather);
    label->setText(filepath);
    vboxLayout->addWidget(label);
    widget->setLayout(vboxLayout);
    m_documment->setWidget(widget);
    qDebug() << "aaa";
    return true;
}

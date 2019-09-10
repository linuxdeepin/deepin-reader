#include "docummentbase.h"
#include <QHBoxLayout>

DocummentBase::DocummentBase(QWidget *parent): QScrollArea(parent)
{
    QHBoxLayout *phblyout = new QHBoxLayout(parent);
    parent->setLayout(phblyout);
    phblyout->addWidget(this);
    m_widget.setLayout(&m_vboxLayout);
    m_widget.setMouseTracking(true);
    setMouseTracking(true);

    m_viewmode = ViewMode_SinglePage;
}

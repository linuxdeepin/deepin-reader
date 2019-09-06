#include "docummentbase.h"

DocummentBase::DocummentBase(QWidget *parent): QScrollArea(parent)
{
    QGridLayout *pgrlyout = new QGridLayout(parent);
    pgrlyout->addWidget(this);
    m_widget.setLayout(&m_vboxLayout);
    m_widget.setMouseTracking(true);
}

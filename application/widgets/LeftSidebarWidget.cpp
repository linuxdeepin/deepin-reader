#include "LeftSidebarWidget.h"

LeftSidebarWidget::LeftSidebarWidget(DWidget *parent):
    DWidget (parent)
{
    setFixedWidth(200);

    m_pVBoxLayout = new QVBoxLayout;

    this->setLayout(m_pVBoxLayout);

    initOperationWidget();
}

void LeftSidebarWidget::initOperationWidget()
{
    m_pStackedWidget = new QStackedWidget;
    m_pVBoxLayout->addWidget(m_pStackedWidget, 0,  Qt::AlignTop);

    m_operationWidget = new MainOperationWidget;
    m_pVBoxLayout->addWidget(m_operationWidget, 0, Qt::AlignBottom);
}

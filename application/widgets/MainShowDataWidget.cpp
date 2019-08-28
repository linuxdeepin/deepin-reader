#include "MainShowDataWidget.h"

MainShowDataWidget::MainShowDataWidget(DWidget *parent) :
    DWidget (parent)
{
    m_mainLayout = new QVBoxLayout;

    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    this->setLayout(m_mainLayout);

    initWidgets();
}

void MainShowDataWidget::initWidgets()
{
    m_pLeftShowWidget = new LeftSidebarWidget;
    m_mainLayout->addWidget(m_pLeftShowWidget, 0, Qt::AlignLeft);
}

void MainShowDataWidget::setSidebarVisible(const bool &bVis ) const
{
    m_pLeftShowWidget->setVisible(bVis);
}

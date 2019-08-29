#include "LeftSidebarWidget.h"

LeftSidebarWidget::LeftSidebarWidget(DWidget *parent):
    DWidget (parent)
{
    setFixedWidth(200);

    m_pVBoxLayout = new QVBoxLayout;

    this->setLayout(m_pVBoxLayout);

    initOperationWidget();

    this->setVisible(false);    //  默认 隐藏

    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
}

LeftSidebarWidget::~LeftSidebarWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

void LeftSidebarWidget::initOperationWidget()
{
    m_pStackedWidget = new DStackedWidget ;
    m_pVBoxLayout->addWidget(m_pStackedWidget, 0,  Qt::AlignTop);

    m_operationWidget = new MainOperationWidget;
    m_pVBoxLayout->addWidget(m_operationWidget, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::update(const QString &)
{
    return 0;
}

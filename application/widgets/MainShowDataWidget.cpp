#include "MainShowDataWidget.h"
#include <DSplitter>
#include <QDebug>

MainShowDataWidget::MainShowDataWidget(DWidget *parent) :
    DWidget (parent)
{
    m_mainLayout = new QVBoxLayout;

    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    this->setLayout(m_mainLayout);

    initWidgets();

    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
}

MainShowDataWidget::~MainShowDataWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

void MainShowDataWidget::initWidgets()
{
//    DSplitter *splitterMain = new QSplitter(Qt::Horizontal);
    m_pLeftShowWidget = new LeftSidebarWidget();
//    splitterMain->addWidget(m_pLeftShowWidget);

//    DWidget *pLeftShowWidget = new DWidget();
//    splitterMain->addWidget(pLeftShowWidget);

//    splitterMain->setStretchFactor(0,0);

    m_mainLayout->addWidget(m_pLeftShowWidget, 0, Qt::AlignLeft);
}

//  侧边栏的显隐
void MainShowDataWidget::setSidebarVisible(const bool &bVis ) const
{
    m_pLeftShowWidget->setVisible(bVis);
}

//  文档显示区域的 手势鼠标状态
void MainShowDataWidget::setFileHandShapeState(const bool &bState) const
{
    qDebug() << "setFileHandShapeState      "  << bState;
}

//  文档显示区域的 放大镜状态
void MainShowDataWidget::setFileMagnifyingState(const bool &bState) const
{
    qDebug() << "setFileMagnifyingState      "  << bState;
}

int MainShowDataWidget::update(const QString &)
{
    return 0;
}

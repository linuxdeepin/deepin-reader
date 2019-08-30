#include "MainShowSplitter.h"
#include <QDebug>

MainShowSplitter::MainShowSplitter(DWidget *parent) :
    DSplitter (parent)
{
    setChildrenCollapsible(false);      //  子部件 不可以被拉伸到 宽度 为0
    initWidgets();

    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
}

MainShowSplitter::~MainShowSplitter()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

void MainShowSplitter::initWidgets()
{
    m_pLeftShowWidget = new LeftSidebarWidget();
    insertWidget(0, m_pLeftShowWidget);

    m_pFileViewWidget = new FileViewWidget();
    connect(this, SIGNAL(sigFileMagnifyingState(const bool&)), m_pFileViewWidget, SLOT(setMagnifyingState(const bool&)));
    insertWidget(1, m_pFileViewWidget);

    //  布局 占比
    setStretchFactor(0, 2);
    setStretchFactor(1, 8);
}

//  侧边栏的显隐
void MainShowSplitter::setSidebarVisible(const bool &bVis ) const
{
    m_pLeftShowWidget->setVisible(bVis);
}

//  文档显示区域的 手势鼠标状态
void MainShowSplitter::setFileHandShapeState(const bool &bState) const
{
    qDebug() << "setFileHandShapeState      "  << bState;
}

//  文档显示区域的 放大镜状态
//void MainShowSplitter::setFileMagnifyingState(const bool &bState) const
//{
//    qDebug() << "setFileMagnifyingState      "  << bState;
//}

int MainShowSplitter::update(const QString &)
{
    return 0;
}

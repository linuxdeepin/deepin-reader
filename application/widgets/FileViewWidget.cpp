#include "FileViewWidget.h"

FileViewWidget::FileViewWidget(DWidget *parent) :
    DWidget (parent)
{
    m_pMsgSubject = MsgSubject::getInstance();
    if(m_pMsgSubject)
    {
        m_pMsgSubject->addObserver(this);
    }

    setMouseTracking(true);     //  接受 鼠标滑动事件

    m_pMagnifyingWidget = new MagnifyingWidget(this);   //  放大镜 窗口
}

FileViewWidget::~FileViewWidget()
{
    if(m_pMsgSubject)
    {
        m_pMsgSubject->removeObserver(this);
    }
}

void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(m_pMagnifyingWidget && m_bCanVisible)
    {
        m_pMagnifyingWidget->setVisible(true);
        QPoint oldPos = event->pos();

        //  鼠标  在放大镜 的中心
        QPoint pos(oldPos.x() - m_pMagnifyingWidget->width()/2 , oldPos.y()-m_pMagnifyingWidget->height()/2);

        m_pMagnifyingWidget->move(pos.x(), pos.y());
    }

    DWidget::mouseMoveEvent(event);
}

//  鼠标 离开 文档显示区域
void FileViewWidget::leaveEvent(QEvent *event)
{
    if(m_pMagnifyingWidget && m_bCanVisible )
    {
        m_pMagnifyingWidget->setVisible(false);
    }

    DWidget::leaveEvent(event);
}

//  放大镜的显隐 标志位
void FileViewWidget::setMagnifyingState(const bool &bVis)
{
    if(!bVis)
    {
        m_bCanVisible = false;      //  放大镜效果关闭
        m_pMagnifyingWidget->setVisible(bVis);
    }
    else
    {
        m_bCanVisible = true;       //  放大镜效果开启
    }
}

int FileViewWidget::update(const int&, const QString &)
{
    return 0;
}

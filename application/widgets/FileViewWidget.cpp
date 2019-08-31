#include "FileViewWidget.h"
#include "header/MsgHeader.h"
#include <QDebug>

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget(parent)
{
    setMouseTracking(true); //  接受 鼠标滑动事件

    m_pMagnifyingWidget = new MagnifyingWidget(this); //  放大镜 窗口
}

void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pMagnifyingWidget && m_bCanVisible) {
        m_pMagnifyingWidget->setVisible(true);
        QPoint oldPos = event->pos();

        //  鼠标  在放大镜 的中心
        QPoint pos(oldPos.x() - m_pMagnifyingWidget->width() / 2, oldPos.y() - m_pMagnifyingWidget->height() / 2);

        m_pMagnifyingWidget->move(pos.x(), pos.y());
    }

    DWidget::mouseMoveEvent(event);
}

//  鼠标 离开 文档显示区域
void FileViewWidget::leaveEvent(QEvent *event)
{
    if (m_pMagnifyingWidget && m_bCanVisible) {
        m_pMagnifyingWidget->setVisible(false);
    }

    DWidget::leaveEvent(event);
}

//  消息 数据 处理
int FileViewWidget::update(const int &msgType, const QString &msgContent)
{
    //  放大镜 的控制
    if (msgType == MSG_MAGNIFYING) {
        int nRes = msgContent.toInt();
        m_bCanVisible = nRes;
        return ConstantMsg::g_effective_res;
    }

    //  手势 信号
    if (msgType == MSG_HANDLESHAPE) {
        int nRes = msgContent.toInt();
        return ConstantMsg::g_effective_res;
    }
    //  更新主题
    if (msgType == MSG_UPDATE_THEME) {
    }
    return 0;
}

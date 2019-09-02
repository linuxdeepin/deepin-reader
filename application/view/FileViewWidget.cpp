#include "FileViewWidget.h"
#include <QDebug>

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget(parent)
{
    setMouseTracking(true); //  接受 鼠标滑动事件

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(SlotCustomContextMenuRequested(const QPoint &)));


    initWidget();
}

FileViewWidget::~FileViewWidget()
{
    if (m_pDefaultOperationWidget) {
        m_pDefaultOperationWidget->deleteLater();
        m_pDefaultOperationWidget = nullptr;
    }

    if (m_pTextOperationWidget) {
        m_pTextOperationWidget->deleteLater();
        m_pTextOperationWidget = nullptr;
    }
}

void FileViewWidget::initWidget()
{
    m_pMagnifyingWidget = new MagnifyingWidget(this); //  放大镜 窗口
    connect(this, SIGNAL(sigSetMagnifyingImage(const QImage &)), m_pMagnifyingWidget, SLOT(setShowImage(const QImage &)));

    m_pDefaultOperationWidget = new DefaultOperationWidget;
    m_pTextOperationWidget = new  TextOperationWidget;
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

//  弹出 自定义 菜单
void FileViewWidget::SlotCustomContextMenuRequested(const QPoint &point)
{
    QPoint clickPos = this->mapToGlobal(point);

    m_pTextOperationWidget->show();
    m_pTextOperationWidget->move(clickPos.x(), clickPos.y());
    m_pTextOperationWidget->raise();
    /*
    m_pDefaultOperationWidget->show();
    m_pDefaultOperationWidget->move(clickPos.x(), clickPos.y());
    m_pDefaultOperationWidget->raise();
    */
}

//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    //  打开文件， 内容为  文件 路径
    if (msgType == MSG_OPEN_FILE_PATH) {
        qDebug() << MSG_OPEN_FILE_PATH <<  "       " << msgContent;
        return ConstantMsg::g_effective_res;
    }
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

    //  放映
    if (msgType == MSG_OPERATION_SCREENING) {
        qDebug() << "       MSG_OPERATION_SCREENING";
        return ConstantMsg::g_effective_res;
    }

    //  放大
    if (msgType == MSG_OPERATION_LARGER) {
        qDebug() << "       MSG_OPERATION_LARGER";
        return ConstantMsg::g_effective_res;
    }

    //  缩小
    if (msgType == MSG_OPERATION_SMALLER) {
        qDebug() << "       MSG_OPERATION_SMALLER";
        return ConstantMsg::g_effective_res;
    }

    //  更新主题
    if (msgType == MSG_UPDATE_THEME) {
    }
    return 0;
}

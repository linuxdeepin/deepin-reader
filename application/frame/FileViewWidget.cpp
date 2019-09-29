#include "FileViewWidget.h"
#include <QGridLayout>
#include <QClipboard>
#include "controller/DataManager.h"
#include "application.h"

#include "docview/docummentproxy.h"
#include <QPrinter>
#include <QPrintDialog>
#include <DMessageBox>
#include "translator/Frame.h"

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget("FileViewWidget", parent)
{
    setMouseTracking(true); //  接受 鼠标滑动事件
    setContextMenuPolicy(Qt::CustomContextMenu);

    initWidget();
    initConnections();
}

FileViewWidget::~FileViewWidget()
{

}

void FileViewWidget::initWidget()
{
    //  实际文档类  唯一实例化设置 父窗口
    DocummentProxy::instance(this);

    m_pDocummentFileHelper = new DocummentFileHelper(this);

    setBookMarkStateWidget();
}

//  鼠标移动
void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }

    auto pDocummentProxy = DocummentProxy::instance();
    if (pDocummentProxy) {
        QPoint globalPos = event->globalPos();
        QPoint docGlobalPos = pDocummentProxy->global2RelativePoint(globalPos);
        if (m_nCurrentHandelState == Handel_State) {    //   手型状态下， 按住鼠标左键 位置进行移动
            if (m_bSelectOrMove) {
                QPoint mvPoint = m_pHandleMoveStartPoint - globalPos;
                int mvX = mvPoint.x();
                int mvY = mvPoint.y();

                pDocummentProxy->pageMove(mvX, mvY);

                m_pHandleMoveStartPoint = globalPos;
            }
        } else if (m_nCurrentHandelState == Magnifier_State) {  //  当前是放大镜状态
            pDocummentProxy->showMagnifier(docGlobalPos);
        } else {
            if (m_bSelectOrMove) {  //  鼠标已经按下，　则选中所经过的文字
                m_pMoveEndPoint = docGlobalPos;
                pDocummentProxy->mouseSelectText(m_pStartPoint, m_pMoveEndPoint);
            } else {
                //  首先判断文档划过属性
                Page::Link *pLink  = pDocummentProxy->mouseBeOverLink(docGlobalPos);
                if (pLink) {
                    setCursor(QCursor(Qt::PointingHandCursor));
                } else {
                    if (pDocummentProxy->mouseBeOverText(docGlobalPos))
                        setCursor(QCursor(Qt::IBeamCursor));
                    else {
                        setCursor(QCursor(Qt::ArrowCursor));
                    }
                }
            }
        }
    }
}

//  鼠标左键 按下
void FileViewWidget::mousePressEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }

    Qt::MouseButton nBtn = event->button();
    if (nBtn == Qt::LeftButton) {
        auto pDocummentProxy = DocummentProxy::instance();

        QPoint globalPos = event->globalPos();
        QPoint docGlobalPos = pDocummentProxy->global2RelativePoint(globalPos);

        //  点击的时候　先判断　点击处　　是否有链接之类
        Page::Link *pLink  = pDocummentProxy->mouseBeOverLink(docGlobalPos);
        if (pLink) {
            m_pDocummentFileHelper->onClickPageLink(pLink);
        } else {
            m_bSelectOrMove = true;

            if (m_nCurrentHandelState == Handel_State) {
                m_pHandleMoveStartPoint = globalPos;     //  变成手，　需要的是　相对坐标
            } else if (m_nCurrentHandelState == Default_State) {
                pDocummentProxy->mouseSelectTextClear();  //  清除之前选中的文字高亮
                m_pStartPoint = docGlobalPos;
            }
        }
    }
}

//  鼠标松开
void FileViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE)
        return;

    m_bSelectOrMove = false;
    CustomWidget::mouseReleaseEvent(event);
}

void FileViewWidget::resizeEvent(QResizeEvent *event)
{
    slotSetWidgetAdapt();
    setBookMarkStateWidget();

    CustomWidget::resizeEvent(event);
}

//  弹出 自定义 菜单
void FileViewWidget::slotCustomContextMenuRequested(const QPoint &point)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE)
        return;

    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;

    //  手型状态， 直接返回
    if (m_nCurrentHandelState == Handel_State)
        return;

    auto pDocummentProxy = DocummentProxy::instance();
    if (pDocummentProxy) {
        QString sSelectText =  "";
        pDocummentProxy->getSelectTextString(sSelectText);  //  选择　当前选中下面是否有文字

        QPoint tempPoint = this->mapToGlobal(point);

        bool bookState = m_pBookMarkStateLabel->bChecked();

        if (sSelectText != "") {
            m_pRightClickPoint = pDocummentProxy->global2RelativePoint(tempPoint);

            QString sAnnotationText = "";
            bool bAnno = pDocummentProxy->annotationClicked(m_pRightClickPoint, sAnnotationText);
            //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
            if (m_pTextOperationWidget == nullptr) {
                m_pTextOperationWidget = new TextOperationWidget(this);
            }

            m_pTextOperationWidget->showWidget(tempPoint.x(), tempPoint.y(), bookState, bAnno);
        } else {
            if (m_pDefaultOperationWidget == nullptr) {
                m_pDefaultOperationWidget = new DefaultOperationWidget(this);
            }
            m_pDefaultOperationWidget->showWidget(tempPoint.x(), tempPoint.y(), bookState);
        }
    }
}

//  放大镜　控制
int FileViewWidget::magnifying(const QString &data)
{
    int nRes = data.toInt();
    if (nRes == 1) {
        m_nCurrentHandelState = Magnifier_State;
        this->setCursor(Qt::BlankCursor);
    } else {
        m_nCurrentHandelState = Default_State;
        this->setCursor(Qt::ArrowCursor);
        DocummentProxy::instance()->closeMagnifier();
    }

    return ConstantMsg::g_effective_res;
}

//  手势控制
int FileViewWidget::setHandShape(const QString &data)
{
    int nRes = data.toInt();
    if (nRes == 1) { //  手形
        m_nCurrentHandelState = Handel_State;
        this->setCursor(Qt::OpenHandCursor);
    } else {
        m_nCurrentHandelState = Default_State;
        this->setCursor(Qt::ArrowCursor);
    }

    //  手型 切换 也需要将之前选中的文字清除 选中样式
    DocummentProxy::instance()->mouseSelectTextClear();

    return ConstantMsg::g_effective_res;
}

//  添加高亮颜色
void FileViewWidget::onFileAddAnnotation(const QString &sColor)
{
    DataManager::instance()->setBIsUpdate(true);
    QList<QColor> colorList = {};

    DocummentProxy::instance()->addAnnotation(m_pRightClickPoint, m_pRightClickPoint);
}

//  移除高亮, 有注释 则删除注释
void FileViewWidget::onFileRemoveAnnotation()
{
    DataManager::instance()->setBIsUpdate(true);
    QString sUuid = DocummentProxy::instance()->removeAnnotation(m_pRightClickPoint);
}

//  信号槽　初始化
void FileViewWidget::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotCustomContextMenuRequested(const QPoint &)));

    connect(this, SIGNAL(sigOpenNoteWidget()), this, SLOT(slotOpenNoteWidget()));
    connect(this, SIGNAL(sigWidgetAdapt()), this, SLOT(slotSetWidgetAdapt()));
    connect(this, SIGNAL(sigPrintFile()), this, SLOT(slotPrintFile()));
}

//  打印
void FileViewWidget::slotPrintFile()
{
    QPrinter printer;
    QString printerName = printer.printerName();
    if ( printerName.size() > 0) {
        QPrintDialog printDialog(&printer, this);
        if (printDialog.exec() == QDialog::Accepted) {
            //  print
        }
    } else {
        DMessageBox::warning(nullptr, "", Frame::sPrintErrorNoDevice);
    }
}

//  注释窗口
void FileViewWidget::slotOpenNoteWidget()
{
    if (m_pFileViewNoteWidget == nullptr) {
        m_pFileViewNoteWidget = new FileViewNoteWidget(this);
    }
    m_pFileViewNoteWidget->show();
//    m_pTextOperationWidget->move(tempPoint.x(), tempPoint.y());
//    m_pFileViewNoteWidget->raise();
}

//  设置　窗口　自适应　宽＼高　度
void FileViewWidget::slotSetWidgetAdapt()
{
    if (m_nAdapteState == WIDGET_State) {
        int nWidth = this->width();
        DocummentProxy::instance()->adaptWidthAndShow(nWidth);
    } else if (m_nAdapteState == HEIGHT_State) {
        int nHeight = this->height();
        DocummentProxy::instance()->adaptHeightAndShow(nHeight);
    }
}

//  书签状态
void FileViewWidget::setBookMarkStateWidget()
{
    if (m_pBookMarkStateLabel == nullptr) {
        m_pBookMarkStateLabel = new BookMarkStateLabel(this);
    }
    int nParentWidth = this->width();
    int nWidget = m_pBookMarkStateLabel->width();
    m_pBookMarkStateLabel->move(nParentWidth - nWidget - 20, 0);
    m_pBookMarkStateLabel->show();
    m_pBookMarkStateLabel->raise();
}

//  标题栏的菜单消息处理
int FileViewWidget::dealWithTitleRequest(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_MAGNIFYING:            //  放大镜信号
        return magnifying(msgContent);
    case MSG_HANDLESHAPE:           //  手势 信号
        return setHandShape(msgContent);
    case MSG_SELF_ADAPTE_HEIGHT:    //  自适应　高度
        if (msgContent == "1") {
            m_nAdapteState = HEIGHT_State ;
            emit sigWidgetAdapt();
        } else {
            m_nAdapteState = Default_State;
        }
        return ConstantMsg::g_effective_res;
    case MSG_SELF_ADAPTE_WIDTH:    //   自适应宽度
        if (msgContent == "1") {
            m_nAdapteState = WIDGET_State;
            emit sigWidgetAdapt();
        } else {
            m_nAdapteState = Default_State;
        }
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_PRINT :      //  打印
        emit sigPrintFile();
        return  ConstantMsg::g_effective_res;
    case MSG_FILE_ROTATE:           //  文档旋转了
        emit sigWidgetAdapt();
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

//  文档右键菜单请求处理
int FileViewWidget::dealWithFileMenuRequest(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPERATION_TEXT_ADD_HIGHLIGHTED:    //  高亮显示
        onFileAddAnnotation(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED: //  移除高亮显示
        onFileRemoveAnnotation();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_ADD_ANNOTATION:     //  添加注释
        emit sigOpenNoteWidget();
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    int nRes = dealWithTitleRequest(msgType, msgContent);
    if (nRes != ConstantMsg::g_effective_res) {

        nRes = dealWithFileMenuRequest(msgType, msgContent);
        if (nRes != ConstantMsg::g_effective_res) {

            if (msgType == MSG_NOTIFY_KEY_MSG) {    //  最后一个处理通知消息
//                if (msgContent == "Up") {
//                    sendMsg(MSG_OPERATION_PREV_PAGE);
//                } else if (msgContent == "Down") {
//                    sendMsg(MSG_OPERATION_NEXT_PAGE);
//                }
            }
        }
    }

    return 0;
}

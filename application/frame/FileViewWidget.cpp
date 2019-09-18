#include "FileViewWidget.h"
#include <QDebug>
#include <QMimeData>
#include <QUrl>
#include <QGridLayout>
#include <QClipboard>
#include "controller/DataManager.h"
#include "application.h"

#include "docview/docummentproxy.h"

FileViewWidget::FileViewWidget(CustomWidget *parent)
    : CustomWidget("FileViewWidget", parent)
{
    setMouseTracking(true); //  接受 鼠标滑动事件
    setAcceptDrops(true);
    setContextMenuPolicy(Qt::CustomContextMenu);

    initWidget();
    initConnections();
}

FileViewWidget::~FileViewWidget()
{
    if (m_pFileAttrWidget) {
        m_pFileAttrWidget->deleteLater();
        m_pFileAttrWidget = nullptr;
    }
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

    DocummentProxy *pDocummentProxy = DocummentProxy::instance();
    if (pDocummentProxy) {
        QPoint globalPos = event->globalPos();
        QPoint docGlobalPos = pDocummentProxy->global2RelativePoint(globalPos);
        if (m_nCurrentHandelState == Handel_State) {    //   手型状态下， 按住鼠标左键 位置进行移动
            if (m_bSelectOrMove) {

                QPoint mvPoint = globalPos - m_pMoveStartPoint;
                int mvX = mvPoint.x();
                int mvY = mvPoint.y();

                pDocummentProxy->pageMove(mvX, mvY);
            }
        } else if (m_nCurrentHandelState == Magnifier_State) {  //  当前是放大镜状态
            pDocummentProxy->showMagnifier(docGlobalPos);
        } else {
            if (m_bSelectOrMove) {  //  鼠标已经按下，　则选中所经过的文字
                pDocummentProxy->mouseSelectText(m_pStartPoint, docGlobalPos);
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

//  鼠标左键 按下
void FileViewWidget::mousePressEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }

    Qt::MouseButton nBtn = event->button();
    if (nBtn == Qt::LeftButton) {

        m_bSelectOrMove = true;

        if (m_nCurrentHandelState == Handel_State) {
            m_pMoveStartPoint = event->globalPos();     //  变成手，　需要的是　相对坐标
        } else if (m_nCurrentHandelState == Default_State) {
            DocummentProxy *pDocummentProxy = DocummentProxy::instance();
            pDocummentProxy->mouseSelectTextClear();  //  清除之前选中的文字高亮
            m_pStartPoint = pDocummentProxy->global2RelativePoint(event->globalPos());
        }
    }
}

//  鼠标松开
void FileViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }
    m_bSelectOrMove = false;
    CustomWidget::mouseReleaseEvent(event);
}

//文件拖拽
void FileViewWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drag event if mime type is url.
    event->accept();
}

void FileViewWidget::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasUrls()) {
        for (auto url : mimeData->urls()) {
            QString sFilePath =  url.toLocalFile();
            if (sFilePath.endsWith(".pdf")) {
                //  默认打开第一个
                QString sRes = sFilePath + "@#&wzx";

                sendMsg(MSG_OPEN_FILE_PATH, sRes);

                break;
            }
        }
    }
}

void FileViewWidget::resizeEvent(QResizeEvent *event)
{
    if (m_pFindWidget != nullptr) {
        int nParentWidth = this->width();
        int nWidget = m_pFindWidget->width();
        m_pFindWidget->move(nParentWidth - nWidget - 20, 20);
    }

    setBookMarkStateWidget();

    CustomWidget::resizeEvent(event);
}

//  弹出 自定义 菜单
void FileViewWidget::slotCustomContextMenuRequested(const QPoint &point)
{
    //  处于幻灯片模式下
    if (DataManager::instance()->CurShowState() == FILE_SLIDE) {
        return;
    }
    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;
    //  手型状态， 直接返回
    if (m_nCurrentHandelState == Handel_State)
        return;
    DocummentProxy *pDocummentProxy = DocummentProxy::instance();

    QPoint clickPos = this->mapToGlobal(point);
    QPoint globalPos = pDocummentProxy->global2RelativePoint(clickPos);
    bool rl = pDocummentProxy->mouseBeOverText(globalPos);
    if (rl) {
        //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
        if (m_pTextOperationWidget == nullptr) {
            m_pTextOperationWidget = new TextOperationWidget(this);
        }
        m_pTextOperationWidget->show();
        m_pTextOperationWidget->move(clickPos.x(), clickPos.y());
        m_pTextOperationWidget->raise();
    } else {
        if (m_pDefaultOperationWidget == nullptr) {
            m_pDefaultOperationWidget = new DefaultOperationWidget(this);
        }
        m_pDefaultOperationWidget->show();
        m_pDefaultOperationWidget->move(clickPos.x(), clickPos.y());
        m_pDefaultOperationWidget->raise();
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
        //  取消放大镜显示
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

void FileViewWidget::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(slotCustomContextMenuRequested(const QPoint &)));

    connect(this, SIGNAL(sigShowFileAttr()), this, SLOT(slotShowFileAttr()));
    connect(this, SIGNAL(sigShowFileFind()), this, SLOT(slotShowFindWidget()));
}

void FileViewWidget::setBookMarkStateWidget()
{
    if (m_pBookMarkStateLabel == nullptr) {
        m_pBookMarkStateLabel = new BookMarkStateLabel(this);
        connect(this, SIGNAL(sigSetMarkState(const bool &)),
                m_pBookMarkStateLabel, SLOT(SlotSetMarkState(const bool &)));
    }
    int nParentWidth = this->width();
    int nWidget = m_pBookMarkStateLabel->width();
    m_pBookMarkStateLabel->move(nParentWidth - nWidget - 20, 0);
    m_pBookMarkStateLabel->show();
    m_pBookMarkStateLabel->raise();
}

//  查看 文件属性
void FileViewWidget::slotShowFileAttr()
{
    //  获取文件的基本数据，　进行展示
    if (m_pFileAttrWidget == nullptr) {
        m_pFileAttrWidget = new FileAttrWidget;
    }
    m_pFileAttrWidget->showScreenCenter();
}

//  显示搜索框
void FileViewWidget::slotShowFindWidget()
{
    if (m_pFindWidget == nullptr) {
        m_pFindWidget = new FindWidget(this);
    }

    int nParentWidth = this->width();
    int nWidget = m_pFindWidget->width();
    m_pFindWidget->move(nParentWidth - nWidget - 20, 20);

    m_pFindWidget->show();
    m_pFindWidget->raise();
}

//  标题栏的菜单消息处理
int FileViewWidget::dealWithTitleMenuRequest(const int &msgType, const QString &)
{
    switch (msgType) {
    case MSG_OPERATION_PRINT :      //  打印
        return  ConstantMsg::g_effective_res;
    case MSG_OPERATION_ATTR:        //  打开该文件的属性信息
        emit sigShowFileAttr();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_FIND:        //  搜索
        emit sigShowFileFind();
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

//  文档右键菜单请求处理
int FileViewWidget::dealWithFileMenuRequest(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPERATION_TEXT_ADD_HIGHLIGHTED:    //  高亮显示
        qDebug() << "   MSG_OPERATION_TEXT_ADD_HIGHLIGHTED  " << msgContent;
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED: //  移除高亮显示
        qDebug() << "   MSG_OPERATION_TEXT_REMOVE_HIGHLIGHTED  ";
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_ADD_ANNOTATION:     //  添加注释
        qDebug() << "   MSG_OPERATION_TEXT_ADD_ANNOTATION  ";
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_MAGNIFYING:            //  放大镜信号
        return magnifying(msgContent);
    case MSG_HANDLESHAPE:           //  手势 信号
        return setHandShape(msgContent);
    case MSG_BOOKMARK_STATE :       //  当前页的书签状态
        emit sigSetMarkState(msgContent.toInt());
        return ConstantMsg::g_effective_res;
    }

    int nRes = dealWithTitleMenuRequest(msgType, msgContent);
    if (nRes != ConstantMsg::g_effective_res) {

        nRes = dealWithFileMenuRequest(msgType, msgContent);
        if (nRes != ConstantMsg::g_effective_res) {

            if (msgType == MSG_NOTIFY_KEY_MSG) {    //  最后一个处理通知消息
                if (msgContent == "Up") {
                    sendMsg(MSG_OPERATION_PREV_PAGE);
                } else if (msgContent == "Down") {
                    sendMsg(MSG_OPERATION_NEXT_PAGE);
                } else if (msgContent == "Esc") {
                }
            }
        }
    }

    return 0;
}

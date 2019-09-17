#include "FileViewWidget.h"
#include <QDebug>
#include <QMimeData>
#include <QUrl>
#include <QGridLayout>
#include "controller/DataManager.h"
#include "application.h"

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
    if (m_pDefaultOperationWidget) {
        m_pDefaultOperationWidget->deleteLater();
        m_pDefaultOperationWidget = nullptr;
    }

    if (m_pTextOperationWidget) {
        m_pTextOperationWidget->deleteLater();
        m_pTextOperationWidget = nullptr;
    }

    if (m_pFileAttrWidget) {
        m_pFileAttrWidget->deleteLater();
        m_pFileAttrWidget = nullptr;
    }
}

void FileViewWidget::initWidget()
{
    //  实际文档类  唯一实例化设置 父窗口
    m_pDocummentProxy = DocummentProxy::instance(this);

    setBookMarkStateWidget();

    m_pDefaultOperationWidget = new DefaultOperationWidget;
    m_pTextOperationWidget = new TextOperationWidget;
    m_pFileAttrWidget = new FileAttrWidget;

    m_pFindWidget = new FindWidget(this);
    int nParentWidth = this->width();
    int nWidget = m_pFindWidget->width();
    m_pFindWidget->move(nParentWidth - nWidget - 20, 20);
}

//  鼠标移动
void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_pDocummentProxy) {
        QPoint globalPos = event->globalPos();
        QPoint docGlobalPos = m_pDocummentProxy->global2RelativePoint(globalPos);
        if (m_nCurrentHandelState == Handel_State) {    //   手型状态下， 按住鼠标左键 位置进行移动
            if (m_bSelectOrMove) {

                QPoint mvPoint = globalPos - m_pMoveStartPoint;
                int mvX = mvPoint.x();
                int mvY = mvPoint.y();

                m_pDocummentProxy->pageMove(mvX, mvY);
            }
        } else if (m_nCurrentHandelState == Magnifier_State) {  //  当前是放大镜状态
            m_pDocummentProxy->showMagnifier(docGlobalPos);
        } else {
            if (m_bSelectOrMove) {
                m_pDocummentProxy->mouseSelectText(m_pStartPoint, docGlobalPos);
            } else {
                if (m_pDocummentProxy->mouseBeOverText(docGlobalPos))
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
    Qt::MouseButton nBtn = event->button();
    if (nBtn == Qt::LeftButton) {
        if (m_pDocummentProxy) {
            m_bSelectOrMove = true;
            if (m_nCurrentHandelState == Handel_State) {
                m_pMoveStartPoint = event->globalPos();     //  变成手，　需要的是　相对坐标
            } else if (m_nCurrentHandelState == Default_State) {
                m_pDocummentProxy->mouseSelectTextClear();  //  清除之前选中的文字高亮
                m_pStartPoint = m_pDocummentProxy->global2RelativePoint(event->globalPos());
            }
        }
    }
}

//  鼠标松开
void FileViewWidget::mouseReleaseEvent(QMouseEvent *event)
{
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

                onOpenFile(sFilePath);

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

//  实际打开文件操作
void FileViewWidget::onOpenFile(const QString &filePath)
{
    if (nullptr != m_pDocummentProxy) {
        bool rl = m_pDocummentProxy->openFile(DocType_PDF, filePath);
        if (rl) {
            DataManager::instance()->setStrOnlyFilePath(filePath);

            m_pDocummentProxy->scaleRotateAndShow(2, RotateType_Normal);

            //  通知 其他窗口， 打开文件成功了！！！
            NotifySubject::getInstance()->sendMsg(MSG_OPERATION_OPEN_FILE_OK);

        } else {
            sendMsg(MSG_OPERATION_OPEN_FILE_FAIL);
        }
    }
}

//  弹出 自定义 菜单
void FileViewWidget::SlotCustomContextMenuRequested(const QPoint &point)
{
    //  放大镜状态， 直接返回
    if (m_nCurrentHandelState == Magnifier_State)
        return;
    if (m_nCurrentHandelState == Handel_State)
        return;

    QPoint clickPos = this->mapToGlobal(point);
    QPoint globalPos = m_pDocummentProxy->global2RelativePoint(clickPos);

    bool rl = m_pDocummentProxy->mouseBeOverText(globalPos);
    if (rl) {
        //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
        m_pTextOperationWidget->show();
        m_pTextOperationWidget->move(clickPos.x(), clickPos.y());
        m_pTextOperationWidget->raise();
    } else {
        m_pDefaultOperationWidget->show();
        m_pDefaultOperationWidget->move(clickPos.x(), clickPos.y());
        m_pDefaultOperationWidget->raise();
    }
}

//  打开　文件路径
void FileViewWidget::openFilePath(const QString &filePaths)
{
    QStringList fileList = filePaths.split("@#&wzx",  QString::SkipEmptyParts);
    int nSize = fileList.size();
    if (nSize > 0) {
        QString sPath = fileList.at(0);
        onOpenFile(sPath);
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
        m_pDocummentProxy->closeMagnifier();
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
    m_pDocummentProxy->mouseSelectTextClear();

    return ConstantMsg::g_effective_res;
}

//  放映
int FileViewWidget::screening(const QString &data)
{
    m_pDocummentProxy->showSlideModel();    //  开启幻灯片
    return ConstantMsg::g_effective_res;
}

void FileViewWidget::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(SlotCustomContextMenuRequested(const QPoint &)));

    connect(this, SIGNAL(sigOpenFile(const QString &)),
            this, SLOT(openFilePath(const QString &)));
}

//  标题栏的菜单消息处理
int FileViewWidget::dealWithTitleMenuRequest(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPEN_FILE_PATH:        //  打开文件
        emit sigOpenFile(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SAVE_FILE : //   保存文件
        qDebug() << "   MSG_OPERATION_SAVE_FILE ";
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SAVE_AS_FILE: // 另存为文件
        qDebug() << "   MSG_OPERATION_SAVE_AS_FILE ";
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_PRINT :      //  打印
        return  ConstantMsg::g_effective_res;
    case MSG_OPERATION_ATTR:        //  打开该文件的属性信息
        onShowFileAttr();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_FIND:        //  搜索
        onShowFindWidget();
        return ConstantMsg::g_effective_res;
//    case MSG_OPERATION_FULLSCREEN:  //  全屏
//        this->setWindowState(Qt::WindowFullScreen);
//        break;
//        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SCREENING:   //  放映
        return screening(msgContent);
    }
    return 0;
}

//  文档右键菜单请求处理
int FileViewWidget::dealWithFileMenuRequest(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPERATION_ADD_BOOKMARK:            //  添加书签
        qDebug() << "   MSG_OPERATION_ADD_BOOKMARK  ";
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_FIRST_PAGE:              //  第一页
        qDebug() << "   MSG_OPERATION_FIRST_PAGE  ";
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_PREV_PAGE:               //  上一页
        qDebug() << "   MSG_OPERATION_PREV_PAGE  ";
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_NEXT_PAGE:               //  下一页
        qDebug() << "   MSG_OPERATION_NEXT_PAGE  ";
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_END_PAGE:                //  最后一页
        qDebug() << "   MSG_OPERATION_END_PAGE  ";
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_COPY:               //  复制
        qDebug() << "   MSG_OPERATION_TEXT_COPY  ";
        return ConstantMsg::g_effective_res;
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
void FileViewWidget::onShowFileAttr()
{
    //  获取文件的基本数据，　进行展示
    m_pFileAttrWidget->showScreenCenter();
}

//  显示搜索框
void FileViewWidget::onShowFindWidget()
{
    m_pFindWidget->show();
    m_pFindWidget->activateWindow();
    m_pFindWidget->raise();
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
                qDebug() << "   MSG_NOTIFY_MSG      " << msgContent;
            }
        }
    }

    return 0;
}

#include "FileViewWidget.h"
#include <QDebug>
#include <QMimeData>
#include <QUrl>
#include <QGridLayout>
#include <DDesktopServices>
#include "controller/DataManager.h"

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
    m_pDefaultOperationWidget = new DefaultOperationWidget;
    m_pTextOperationWidget = new  TextOperationWidget;
    m_pFileAttrWidget = new FileAttrWidget;

    m_pFindWidget = new FindWidget(this);
    int nParentWidth = this->width();
    int nWidget = m_pFindWidget->width();
    m_pFindWidget->move(nParentWidth - nWidget - 20, 20);

    m_pDocummentProxy = new DocummentProxy(this);
}

void FileViewWidget::mouseMoveEvent(QMouseEvent *event)
{
    DWidget::mouseMoveEvent(event);
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
            onOpenFile(url.toLocalFile());
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

    CustomWidget::resizeEvent(event);
}

//  实际打开文件操作
void FileViewWidget::onOpenFile(const QString &filePath)
{
    if (nullptr != m_pDocummentProxy) {
        bool rl = m_pDocummentProxy->openFile(DocType_PDF, filePath);
        if (rl) {
            DataManager::instance()->setStrOnlyFilePath(filePath);

            m_pDocummentProxy->scaleRotateAndShow(1, RotateType_Normal);

            NotifySubject::getInstance()->sendMsg(MSG_OPERATION_OPEN_FILE_OK);
        } else {
            sendMsg(MSG_OPERATION_OPEN_FILE_FAIL);
        }
    }
}

//  弹出 自定义 菜单
void FileViewWidget::SlotCustomContextMenuRequested(const QPoint &point)
{
    QPoint clickPos = this->mapToGlobal(point);

    //  需要　区别　当前选中的区域，　弹出　不一样的　菜单选项
//    m_pTextOperationWidget->show();
//    m_pTextOperationWidget->move(clickPos.x(), clickPos.y());
//    m_pTextOperationWidget->raise();

    m_pDefaultOperationWidget->show();
    m_pDefaultOperationWidget->move(clickPos.x(), clickPos.y());
    m_pDefaultOperationWidget->raise();

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
    return ConstantMsg::g_effective_res;
}

//  手势控制
int FileViewWidget::setHandShape(const QString &data)
{
    int nRes = data.toInt();

    return ConstantMsg::g_effective_res;
}

//  放映
int FileViewWidget::screening(const QString &data)
{
    return ConstantMsg::g_effective_res;
}

void FileViewWidget::initConnections()
{
    connect(this, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(SlotCustomContextMenuRequested(const QPoint &)));

    connect(this, SIGNAL(sigOpenFile(const QString &)), this, SLOT(openFilePath(const QString &)));
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
    m_pFindWidget->raise();
}

//  打开文件所在文件夹
int FileViewWidget::openFileFolder()
{
    QString strFilePath = DataManager::instance()->strOnlyFilePath();
    if (strFilePath != "") {
        int nLastPos = strFilePath.lastIndexOf('/');
        strFilePath = strFilePath.mid(0, nLastPos);
        DDesktopServices::showFolder(strFilePath);
    }
    return ConstantMsg::g_effective_res;
}

//  消息 数据 处理
int FileViewWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPEN_FILE_PATH:        //  打开文件
        emit sigOpenFile(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_ATTR:        //  打开该文件的属性信息
        onShowFileAttr();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_FIND:        //  搜索
        onShowFindWidget();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_OPEN_FOLDER: //  打开该文件所处文件夹
        return openFileFolder();
    case MSG_MAGNIFYING:            //  放大镜信号
        return magnifying(msgContent);
    case MSG_HANDLESHAPE:           //  手势 信号
        return setHandShape(msgContent);
    case MSG_OPERATION_SCREENING:   //  放映
        return screening(msgContent);
    case MSG_OPERATION_LARGER:      //  放大
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SMALLER:     //  缩小
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_FIRST_PAGE:  //  第一页
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_PREV_PAGE:   //  上一页
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_NEXT_PAGE:   //  下一页
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_END_PAGE:    //  最后一页
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

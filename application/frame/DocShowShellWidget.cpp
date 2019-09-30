#include "DocShowShellWidget.h"
#include <QVBoxLayout>
#include <QMimeData>
#include <QUrl>
#include "FileViewWidget.h"
#include <QFileInfo>

DocShowShellWidget::DocShowShellWidget(CustomWidget *parent)
    : CustomWidget ("DocShowShellWidget", parent)
{
    setAcceptDrops(true);

    initWidget();
    initConnections();
}

DocShowShellWidget::~DocShowShellWidget()
{
    if (m_pFileAttrWidget) {
        m_pFileAttrWidget->deleteLater();
        m_pFileAttrWidget = nullptr;
    }
}

//文件拖拽
void DocShowShellWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drag event if mime type is url.
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->accept();
    }
}

void DocShowShellWidget::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        for (auto url : mimeData->urls()) {
            QString sFilePath =  url.toLocalFile();

            QFileInfo info(sFilePath);
            QString sCompleteSuffix = info.completeSuffix();    //  文件后缀
            if (sCompleteSuffix == "pdf" || sCompleteSuffix == "tiff") {
                //  默认打开第一个
                QString sRes = sFilePath + Constant::sQStringSep;

                sendMsg(MSG_OPEN_FILE_PATH, sRes);

                break;
            }
        }
    }
}

void DocShowShellWidget::resizeEvent(QResizeEvent *event)
{
    if (m_pFindWidget != nullptr) {
        int nParentWidth = this->width();
        int nWidget = m_pFindWidget->width();
        m_pFindWidget->move(nParentWidth - nWidget - 20, 20);
    }

    if (m_pFileViewNoteWidget != nullptr) {
        int nParentWidth = this->width();
        int nWidget = m_pFileViewNoteWidget->width();
        m_pFileViewNoteWidget->move(nParentWidth - nWidget - 50, 200);
    }

    setBookMarkStateWidget();

    CustomWidget::resizeEvent(event);
}

//  获取文件的基本数据，　进行展示
void DocShowShellWidget::slotShowFileAttr()
{
    if (m_pFileAttrWidget == nullptr) {
        m_pFileAttrWidget = new FileAttrWidget;
    }
    m_pFileAttrWidget->showScreenCenter();
}

//  搜索框
void DocShowShellWidget::slotShowFindWidget()
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

//  注释窗口
void DocShowShellWidget::slotOpenNoteWidget(const QString &sPoint)
{
    if (m_pFileViewNoteWidget == nullptr) {
        m_pFileViewNoteWidget = new FileViewNoteWidget(this);
    }

    int nParentWidth = this->width();
    int nWidth = m_pFileViewNoteWidget->width();

    QStringList ssPointList = sPoint.split(",,,,", QString::SkipEmptyParts);

    QString sSelectText = ssPointList.at(1); //  选中添加注释的文字

    m_pFileViewNoteWidget->move(nParentWidth - nWidth - 50, 200);
    m_pFileViewNoteWidget->show();
    m_pFileViewNoteWidget->raise();
}


void DocShowShellWidget::initConnections()
{
    connect(this, SIGNAL(sigShowFileAttr()), this, SLOT(slotShowFileAttr()));
    connect(this, SIGNAL(sigShowFileFind()), this, SLOT(slotShowFindWidget()));

    connect(this, SIGNAL(sigOpenNoteWidget(const QString &)), this, SLOT(slotOpenNoteWidget(const QString &)));
}

int DocShowShellWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
    case MSG_OPERATION_ATTR:        //  打开该文件的属性信息
        emit sigShowFileAttr();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_FIND:        //  搜索
        emit sigShowFileFind();
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_TEXT_ADD_ANNOTATION:     //  添加注释
        emit sigOpenNoteWidget(msgContent);
        return ConstantMsg::g_effective_res;
    case MSG_NOTIFY_KEY_MSG : {    //  最后一个处理通知消息
        if ("Ctrl+F" == msgContent) {
            emit sigShowFileFind();
            return ConstantMsg::g_effective_res;
        }
    }
    }

    return 0;
}

void DocShowShellWidget::initWidget()
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(new FileViewWidget);

    this->setLayout(layout);

    setBookMarkStateWidget();
}

//  书签状态
void DocShowShellWidget::setBookMarkStateWidget()
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

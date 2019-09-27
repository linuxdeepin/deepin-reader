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
            QString sCompleteSuffix = info.completeSuffix();
            if (sCompleteSuffix == "pdf" || sCompleteSuffix == "tiff") {
                //  默认打开第一个
                QString sRes = sFilePath + "@#&wzx";

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
    CustomWidget::resizeEvent(event);
}

void DocShowShellWidget::slotShowFileAttr()
{
    //  获取文件的基本数据，　进行展示
    if (m_pFileAttrWidget == nullptr) {
        m_pFileAttrWidget = new FileAttrWidget;
    }
    m_pFileAttrWidget->showScreenCenter();
}

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

void DocShowShellWidget::initConnections()
{
    connect(this, SIGNAL(sigShowFileAttr()), this, SLOT(slotShowFileAttr()));
    connect(this, SIGNAL(sigShowFileFind()), this, SLOT(slotShowFindWidget()));
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
}

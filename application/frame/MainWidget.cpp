#include "MainWidget.h"
#include <DSplitter>

#include <QFileInfo>
#include <QMimeData>
#include <QUrl>

#include "HomeWidget.h"
#include "DocShowShellWidget.h"
#include "LeftSidebarWidget.h"
#include <QStackedLayout>

MainWidget::MainWidget(CustomWidget *parent) :
    CustomWidget ("MainWidget", parent)
{
    setAcceptDrops(true);

    initWidget();
    initConnections();
}

void MainWidget::initConnections()
{
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigOpenFileFail(const QString &)), this, SLOT(slotOpenFileFail(const QString &)));
}

//  文件打开成功
void MainWidget::slotOpenFileOk()
{
    auto pLayout = this->findChild<QStackedLayout *>();
    if (pLayout) {
        pLayout->setCurrentIndex(1);
    }
}

//  文件打开失败
void MainWidget::slotOpenFileFail(const QString &errorInfo)
{
    DMessageBox::warning(nullptr, tr("deepin-reader"), errorInfo);
}

int MainWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk();
    } else if (msgType == MSG_OPERATION_OPEN_FILE_FAIL) {
        emit sigOpenFileFail(msgContent);
        return ConstantMsg::g_effective_res;
    }

    return 0;
}

void MainWidget::dragEnterEvent(QDragEnterEvent *event)
{
    // Accept drag event if mime type is url.
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        event->accept();
    }
}

void MainWidget::dropEvent(QDropEvent *event)
{
    auto mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        for (auto url : mimeData->urls()) {
            QString sFilePath =  url.toLocalFile();
            if (sFilePath.endsWith("pdf") || sFilePath.endsWith("tiff") || sFilePath.endsWith("ps") || sFilePath.endsWith("xps") || sFilePath.endsWith("djvu")
               ) {
                //  默认打开第一个
                QString sRes = sFilePath + Constant::sQStringSep;

                sendMsg(MSG_OPEN_FILE_PATH, sRes);

                break;
            }
        }
    }
}

void MainWidget::initWidget()
{
    auto pStcakLayout = new QStackedLayout(this);
    pStcakLayout->setContentsMargins(0, 0, 0, 0);
    pStcakLayout->setSpacing(0);

    pStcakLayout->addWidget(new HomeWidget);

    auto pSplitter = new DSplitter;
    pSplitter->setHandleWidth(5);
    pSplitter->setChildrenCollapsible(false);   //  子部件不可拉伸到 0

    pSplitter->addWidget(new LeftSidebarWidget);
    pSplitter->addWidget(new DocShowShellWidget);

    QList<int> list_src;
    list_src.append(300);
    list_src.append(1000);
    pSplitter->setSizes(list_src);

    pStcakLayout->addWidget(pSplitter);
}

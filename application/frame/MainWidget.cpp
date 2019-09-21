#include "MainWidget.h"
#include <QHBoxLayout>
#include <DStackedWidget>
#include <DSplitter>

#include "HomeWidget.h"
#include "FileViewWidget.h"
#include "LeftSidebarWidget.h"

MainWidget::MainWidget(CustomWidget *parent) :
    CustomWidget ("MainWidget", parent)
{
    initWidget();
}

//  文件打开成功
void MainWidget::openFileOk()
{
    DStackedWidget *pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        pWidget->setCurrentIndex(1);
    }
}

//  文件打开失败
void MainWidget::openFileFail(const QString &errorInfo)
{
    qDebug() << "openFileFail       "   <<  errorInfo;
}

int MainWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        openFileOk();
    }

    if (msgType == MSG_OPERATION_OPEN_FILE_FAIL) {
        openFileFail(msgContent);
        return ConstantMsg::g_effective_res;
    }

    return 0;
}

void MainWidget::initWidget()
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    DStackedWidget *pStackedWidget = new DStackedWidget;
    layout->addWidget(pStackedWidget);

    pStackedWidget->addWidget(new HomeWidget);

    DSplitter *pSplitter = new DSplitter;
    pSplitter->setChildrenCollapsible(false);   //  子部件不可拉伸到 0

    pSplitter->insertWidget(0, new LeftSidebarWidget);
    pSplitter->setStretchFactor(0, 2);

    pSplitter->insertWidget(1, new FileViewWidget);
    pSplitter->setStretchFactor(1, 8);

    pStackedWidget->addWidget(pSplitter);

    pStackedWidget->setCurrentIndex(0);
}

#include "MainWidget.h"
#include <QHBoxLayout>
#include <DStackedWidget>
#include <DSplitter>

#include "HomeWidget.h"
#include "DocShowShellWidget.h"
#include "LeftSidebarWidget.h"

MainWidget::MainWidget(CustomWidget *parent) :
    CustomWidget ("MainWidget", parent)
{
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
    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        pWidget->setCurrentIndex(1);
    }
}

//  文件打开失败
void MainWidget::slotOpenFileFail(const QString &errorInfo)
{
    qDebug() << "openFileFail       "   <<  errorInfo;
}

int MainWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk();
    }

    if (msgType == MSG_OPERATION_OPEN_FILE_FAIL) {
        emit sigOpenFileFail(msgContent);
        return ConstantMsg::g_effective_res;
    }

    return 0;
}

void MainWidget::initWidget()
{
    auto layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    this->setLayout(layout);

    auto pStackedWidget = new DStackedWidget;
    layout->addWidget(pStackedWidget);

    pStackedWidget->addWidget(new HomeWidget);

    auto pSplitter = new DSplitter;
    pSplitter->setHandleWidth(5);
    pSplitter->setChildrenCollapsible(false);   //  子部件不可拉伸到 0

    pSplitter->addWidget(new LeftSidebarWidget);
    pSplitter->setStretchFactor(0, 1);

    pSplitter->addWidget(new DocShowShellWidget);
    pSplitter->setStretchFactor(1, 9);

    pStackedWidget->addWidget(pSplitter);

    pStackedWidget->setCurrentIndex(0);
}

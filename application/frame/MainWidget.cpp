#include "MainWidget.h"
#include <QHBoxLayout>

#include "HomeWidget.h"

#include <DSplitter>
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
    m_pStackedWidget->setCurrentIndex(1);
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

    m_pStackedWidget = new DStackedWidget;
    layout->addWidget(m_pStackedWidget);

    HomeWidget *homeWidget = new HomeWidget;
    m_pStackedWidget->addWidget(homeWidget);

    DSplitter *pSplitter = new  DSplitter;
    pSplitter->setChildrenCollapsible(false);   //  子部件不可拉伸到 0

    LeftSidebarWidget *pLeftShowWidget = new LeftSidebarWidget;
    pSplitter->insertWidget(0, pLeftShowWidget);

    FileViewWidget *pFileViewWidget = new FileViewWidget;
    pSplitter->insertWidget(1, pFileViewWidget);

    //  布局 占比
    pSplitter->setStretchFactor(0, 2);
    pSplitter->setStretchFactor(1, 8);

    m_pStackedWidget->addWidget(pSplitter);

    m_pStackedWidget->setCurrentIndex(0);
}

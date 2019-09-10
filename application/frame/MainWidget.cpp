#include "MainWidget.h"
#include <QHBoxLayout>

#include "HomeWidget.h"
#include "MainShowSplitter.h"

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

    MainShowSplitter *showSplitter = new MainShowSplitter;
    m_pStackedWidget->addWidget(showSplitter);

    m_pStackedWidget->setCurrentIndex(0);
}

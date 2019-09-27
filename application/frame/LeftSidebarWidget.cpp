#include "LeftSidebarWidget.h"
#include <DStackedWidget>
#include <QVBoxLayout>

#include "pdfControl/BookMarkWidget.h"
#include "pdfControl/NotesWidget.h"
#include "pdfControl/ThumbnailWidget.h"
#include "pdfControl/SearchResWidget.h"

#include "MainOperationWidget.h"

LeftSidebarWidget::LeftSidebarWidget(CustomWidget *parent):
    CustomWidget ("LeftSidebarWidget", parent)
{
    setMinimumWidth(250);
    setMaximumWidth(500);

    initWidget();

    this->setVisible(false);    //  默认 隐藏
}

void LeftSidebarWidget::initWidget()
{
    auto pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    this->setLayout(pVBoxLayout);

    auto pStackedWidget = new DStackedWidget;
    pStackedWidget->insertWidget(0, new ThumbnailWidget);
    pStackedWidget->insertWidget(1, new BookMarkWidget);
    pStackedWidget->insertWidget(2, new NotesWidget);
    pStackedWidget->insertWidget(3, new SearchResWidget);
    pStackedWidget->setCurrentIndex(0);

    pVBoxLayout->addWidget(pStackedWidget);
    pVBoxLayout->addWidget(new MainOperationWidget, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_SWITCHLEFTWIDGET) {    //切换页面
        auto pWidget = this->findChild<DStackedWidget *>();
        if (pWidget) {
            int nIndex = msgContent.toInt();
            pWidget->setCurrentIndex(nIndex);
        }
        return ConstantMsg::g_effective_res;
    }
    if (msgType == MSG_SLIDER_SHOW_STATE) { //  控制 侧边栏显隐
        bool bVis = msgContent.toInt();
        setVisible(bVis);
        return  ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_OPERATION_FULLSCREEN) { //  全屏
        this->setVisible(false);
    }

    return 0;
}

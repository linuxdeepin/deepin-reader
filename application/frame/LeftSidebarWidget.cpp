#include "LeftSidebarWidget.h"
#include <DStackedWidget>
#include <QVBoxLayout>

#include "controller/DataManager.h"
#include "pdfControl/BookMarkWidget.h"
#include "pdfControl/NotesWidget.h"
#include "pdfControl/SearchResWidget.h"
#include "pdfControl/ThumbnailWidget.h"

#include "MainOperationWidget.h"
#include "controller/AppSetting.h"

LeftSidebarWidget::LeftSidebarWidget(CustomWidget *parent)
    : CustomWidget("LeftSidebarWidget", parent)
{
    //    setMinimumWidth(226);
    //    setMaximumWidth(452);
    setMinimumWidth(LEFTMINWIDTH);
    setMaximumWidth(LEFTMAXWIDTH);

    initWidget();
    initConnections();

    slotWidgetVisible(0);  //  默认 隐藏
    slotUpdateTheme();
}

void LeftSidebarWidget::slotStackSetCurIndex(const int &iIndex)
{
    AppSetting::instance()->setKeyValue(KEY_WIDGET, QString("%1").arg(iIndex));

    auto pWidget = this->findChild<DStackedWidget *>();
    if (pWidget) {
        if (WIDGET_SEARCH != iIndex) {
            DataManager::instance()->setCurrentWidget(iIndex);
        }
        pWidget->setCurrentIndex(iIndex);
    }
}

void LeftSidebarWidget::slotWidgetVisible(const int &nVis)
{
    this->setVisible(nVis);
}

void LeftSidebarWidget::slotUpdateTheme()
{
    updateWidgetTheme();
}

void LeftSidebarWidget::initConnections()
{
    connect(this, SIGNAL(sigStackSetCurIndex(const int &)), this,
            SLOT(slotStackSetCurIndex(const int &)));
    connect(this, SIGNAL(sigWidgetVisible(const int &)), this,
            SLOT(slotWidgetVisible(const int &)));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
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
    if (msgType == MSG_SWITCHLEFTWIDGET) {  //切换页面
        emit sigStackSetCurIndex(msgContent.toInt());
        return ConstantMsg::g_effective_res;
    }
    if (msgType == MSG_SLIDER_SHOW_STATE) {  //  控制 侧边栏显隐
        emit sigWidgetVisible(msgContent.toInt());
        return ConstantMsg::g_effective_res;
    } else if (msgType == MSG_OPERATION_UPDATE_THEME) {
        emit sigUpdateTheme();
    }

    return 0;
}

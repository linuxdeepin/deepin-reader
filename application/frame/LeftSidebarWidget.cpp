#include "LeftSidebarWidget.h"

LeftSidebarWidget::LeftSidebarWidget(CustomWidget *parent):
    CustomWidget ("LeftSidebarWidget", parent)
{
    setMinimumWidth(250);
    setMaximumWidth(500);

    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);

    this->setLayout(m_pVBoxLayout);

    initOperationWidget();

    this->setVisible(false);    //  默认 隐藏

    connect(this, SIGNAL(sigSwitchWidget(const int)), this, SLOT(slotSwitchWidget(const int)));
}

void LeftSidebarWidget::slotSwitchWidget(const int &index)
{
    m_pStackedWidget->setCurrentIndex(index);
}

void LeftSidebarWidget::initOperationWidget()
{
    m_pStackedWidget = new DStackedWidget;
    m_pVBoxLayout->addWidget(m_pStackedWidget);

    m_pThumbnailWidget = new ThumbnailWidget;
    m_pBookMarkWidget = new BookMarkWidget;
    m_pNotesWidget = new NotesWidget(this, NOTE_WIDGET);
    m_pSearchWidget = new NotesWidget(this, SEARCH_WIDGET);

    m_pStackedWidget->insertWidget(THUMBNAIL, m_pThumbnailWidget);
    m_pStackedWidget->insertWidget(BOOK, m_pBookMarkWidget);
    m_pStackedWidget->insertWidget(NOTE, m_pNotesWidget);
    m_pStackedWidget->insertWidget(SEARCH, m_pSearchWidget);
    m_pStackedWidget->setCurrentIndex(THUMBNAIL);

    m_operationWidget = new MainOperationWidget;
    m_pVBoxLayout->addWidget(m_operationWidget, 0, Qt::AlignBottom);
}

void LeftSidebarWidget::initWidget()
{

}

int LeftSidebarWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_SWITCHLEFTWIDGET) {    //切换页面
        int nIndex = msgContent.toInt();
        emit sigSwitchWidget(nIndex);
        return ConstantMsg::g_effective_res;
    }
    if (msgType == MSG_SLIDER_SHOW_STATE) { //  控制 侧边栏显隐
        bool bVis = msgContent.toInt();
        setVisible(bVis);
        return  ConstantMsg::g_effective_res;
    }

    return 0;
}

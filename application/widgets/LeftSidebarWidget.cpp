#include "LeftSidebarWidget.h"

LeftSidebarWidget::LeftSidebarWidget(CustomWidget *parent):
    CustomWidget (parent)
{
    setMinimumWidth(250);
    setMaximumWidth(500);

    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);

    this->setLayout(m_pVBoxLayout);

    initOperationWidget();

    this->setVisible(false);    //  默认 隐藏
}

void LeftSidebarWidget::initWidget()
{

}

void LeftSidebarWidget::initOperationWidget()
{
    m_pStackedWidget = new DStackedWidget;
    m_pVBoxLayout->addWidget(m_pStackedWidget);

    m_pThumbnailWidget = new ThumbnailWidget;
    m_pBookMarkWidget = new BookMarkWidget;
    m_pNotesWidget = new NotesWidget;

    m_pStackedWidget->insertWidget(THUMBNAIL, m_pThumbnailWidget);
    m_pStackedWidget->insertWidget(BOOK, m_pBookMarkWidget);
    m_pStackedWidget->insertWidget(NOTE, m_pNotesWidget);
    m_pStackedWidget->setCurrentIndex(THUMBNAIL);

    m_operationWidget = new MainOperationWidget;
    m_pVBoxLayout->addWidget(m_operationWidget, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_SWITCHLEFTWIDGET) {    //切换页面
        int nIndex = msgContent.toInt();
        m_pStackedWidget->setCurrentIndex(nIndex);
        return ConstantMsg::g_effective_res;
    }

    return 0;
}

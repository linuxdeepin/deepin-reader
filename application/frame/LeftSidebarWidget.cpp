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
//<<<<<<< Updated upstream
//=======

//    connect(this, SIGNAL(sigSwitchWidget(const int)), this, SLOT(slotSwitchWidget(const int)));
//}

//void LeftSidebarWidget::slotSwitchWidget(const int &index)
//{
//    m_pStackedWidget->setCurrentIndex(index);
//}

//void LeftSidebarWidget::initOperationWidget()
//{
//    m_pStackedWidget = new DStackedWidget;
//    m_pVBoxLayout->addWidget(m_pStackedWidget);

//    m_pThumbnailWidget = new ThumbnailWidget;
//    m_pBookMarkWidget = new BookMarkWidget;
//    m_pNotesWidget = new NotesWidget(NOTE);
//    m_pSearchWidget = new NotesWidget(SEARCH);

//    m_pStackedWidget->insertWidget(THUMBNAIL, m_pThumbnailWidget);
//    m_pStackedWidget->insertWidget(BOOK, m_pBookMarkWidget);
//    m_pStackedWidget->insertWidget(NOTE, m_pNotesWidget);
//    m_pStackedWidget->insertWidget(SEARCH, m_pSearchWidget);
//    m_pStackedWidget->setCurrentIndex(THUMBNAIL);

//    m_operationWidget = new MainOperationWidget;
//    m_pVBoxLayout->addWidget(m_operationWidget, 0, Qt::AlignBottom);
//>>>>>>> Stashed changes
}

void LeftSidebarWidget::initWidget()
{
    QVBoxLayout *pVBoxLayout = new QVBoxLayout;
    pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    pVBoxLayout->setSpacing(0);
    this->setLayout(pVBoxLayout);

    DStackedWidget *m_pStackedWidget = new DStackedWidget;
    pVBoxLayout->addWidget(m_pStackedWidget);
    m_pStackedWidget->insertWidget(0, new ThumbnailWidget);
    m_pStackedWidget->insertWidget(1, new BookMarkWidget);
    m_pStackedWidget->insertWidget(2, new NotesWidget);
    m_pStackedWidget->insertWidget(3, new SearchResWidget);
    m_pStackedWidget->setCurrentIndex(0);

    pVBoxLayout->addWidget(new MainOperationWidget, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_SWITCHLEFTWIDGET) {    //切换页面
        DStackedWidget *pWidget = this->findChild<DStackedWidget *>();
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

    return 0;
}

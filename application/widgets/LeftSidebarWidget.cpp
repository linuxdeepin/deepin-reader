#include "LeftSidebarWidget.h"

LeftSidebarWidget::LeftSidebarWidget(DWidget *parent):
    DWidget (parent)
{
    setFixedWidth(250);

    m_pVBoxLayout = new QVBoxLayout;

    this->setLayout(m_pVBoxLayout);

    initOperationWidget();

    this->setVisible(false);    //  默认 隐藏

    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
    connect(m_pThumbnailWidget, SIGNAL(selectIndexPage(const int &)), m_pPageWidget, SLOT(onSetCurrentPage(const int&)));
    connect(m_pPageWidget, SIGNAL(jumpToIndexPage(const int &)), m_pThumbnailWidget, SLOT(onSetCurrentPage(const int&)));
    connect(m_operationWidget, SIGNAL(showType(const int&)), this, SLOT(showListWidget(const int&)));
}

LeftSidebarWidget::~LeftSidebarWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }

}

void LeftSidebarWidget::showListWidget(const int &index) const
{
    qDebug() << index;
    m_pStackedWidget->setCurrentIndex(index);
}

void LeftSidebarWidget::initOperationWidget()
{
    m_pStackedWidget = new DStackedWidget;
    m_pVBoxLayout->addWidget(m_pStackedWidget, 0,  Qt::AlignTop);
    m_pStackedWidget->setMinimumSize(QSize(250, 500));

    DWidget * t_ThumbnailWidget = new DWidget;
    t_ThumbnailWidget->setMinimumSize(QSize(250, 500));
    QVBoxLayout * t_vLayoutThumbnail = new QVBoxLayout;
    m_pThumbnailWidget = new ThumbnailWidget;
    m_pPageWidget = new PagingWidget;
    t_vLayoutThumbnail->addWidget(m_pThumbnailWidget);
    t_vLayoutThumbnail->addWidget(m_pPageWidget);
    t_ThumbnailWidget->setLayout(t_vLayoutThumbnail);

    m_pBookMarkWidget = new BookMarkForm;
    m_pNotesWidget = new NotesForm;

    m_pStackedWidget->insertWidget(THUMBNAIL,t_ThumbnailWidget);
    m_pStackedWidget->insertWidget(BOOK,m_pBookMarkWidget);
    m_pStackedWidget->insertWidget(NOTE,m_pNotesWidget);
    m_pStackedWidget->setCurrentIndex(THUMBNAIL);

    m_operationWidget = new MainOperationWidget;
    m_pVBoxLayout->addWidget(m_operationWidget, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::update(const QString &)
{
    return 0;
}

#include "LeftSidebarWidget.h"

LeftSidebarWidget::LeftSidebarWidget(DWidget *parent):
    DWidget (parent)
{
    setMinimumWidth(250);
    setMaximumWidth(500);

    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);

    this->setLayout(m_pVBoxLayout);

    initOperationWidget();

    //this->setVisible(false);    //  默认 隐藏

    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
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
    m_pVBoxLayout->addWidget(m_pStackedWidget);

    m_pThumbnailWidget = new ThumbnailWidget;
    m_pBookMarkWidget = new BookMarkWidget;
    m_pNotesWidget = new NotesWidget;

    m_pStackedWidget->insertWidget(THUMBNAIL, m_pThumbnailWidget);
    m_pStackedWidget->insertWidget(BOOK, m_pBookMarkWidget);
    m_pStackedWidget->insertWidget(NOTE, m_pNotesWidget);
    m_pStackedWidget->setCurrentIndex(THUMBNAIL);

    m_operationWidget = new MainOperationWidget;
    connect(m_operationWidget, SIGNAL(showType(const int&)), this, SLOT(showListWidget(const int&)));

    m_pVBoxLayout->addWidget(m_operationWidget, 0, Qt::AlignBottom);
}

int LeftSidebarWidget::update(const QString &)
{
    return 0;
}

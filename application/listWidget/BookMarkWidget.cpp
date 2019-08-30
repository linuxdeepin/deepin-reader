#include "BookMarkWidget.h"

BookMarkWidget::BookMarkWidget(DWidget *parent) :
    DWidget(parent)
{
    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }

    m_pVBoxLayout =new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);

    initWidget();
}

BookMarkWidget::~BookMarkWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
    }
}

void BookMarkWidget::initWidget()
{
    m_pBookMarkListWidget = new DListWidget;
    m_pVBoxLayout->addWidget(m_pBookMarkListWidget);

    for (int page = 0; page < 20; ++page) {
        BookMarkItemWidget * t_widget = new BookMarkItemWidget;
        t_widget->setPicture(QString(":/images/logo_96.svg"));
        t_widget->setPage(QString("页面%1").arg(page + 1));
        t_widget->setMinimumSize(QSize(250, 150));

        QListWidgetItem * item = new QListWidgetItem(m_pBookMarkListWidget);
        item->setFlags(Qt::ItemIsSelectable);
        item->setSizeHint(QSize(250, 150));

        m_pBookMarkListWidget->insertItem(page, item);
        m_pBookMarkListWidget->setItemWidget(item, t_widget);
    }
}

int BookMarkWidget::update(const QString &)
{
    return 0;
}

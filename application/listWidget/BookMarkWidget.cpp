#include "BookMarkWidget.h"

BookMarkWidget::BookMarkWidget(DWidget *parent) :
    DWidget(parent)
{
    m_pMsgSubject = MsgSubject::getInstance();
    if(m_pMsgSubject)
    {
        m_pMsgSubject->addObserver(this);
    }

    m_pVBoxLayout =new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);

    initWidget();
}

BookMarkWidget::~BookMarkWidget()
{
    if(m_pMsgSubject)
    {
        m_pMsgSubject->removeObserver(this);
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

int BookMarkWidget::update(const int&, const QString &)
{
    return 0;
}

#include "BookMarkWidget.h"
#include <QDebug>

BookMarkWidget::BookMarkWidget(CustomWidget *parent) :
    CustomWidget("BookMarkWidget", parent)
{
    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(0);
    this->setLayout(m_pVBoxLayout);

    initWidget();

    connect(m_pBookMarkListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
}

void BookMarkWidget::slotShowSelectItem(QListWidgetItem *item)
{
    m_iCurrentIndex = m_pBookMarkListWidget->row(item);
}

void BookMarkWidget::initWidget()
{
    m_pBookMarkListWidget = new DListWidget;
    m_pBookMarkListWidget->setSpacing(10);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pBookMarkListWidget->setResizeMode(QListWidget::Adjust);
    m_pVBoxLayout->addWidget(m_pBookMarkListWidget);

    for (int page = 0; page < 20; ++page) {
        BookMarkItemWidget *t_widget = new BookMarkItemWidget;
        t_widget->setPicture(QString(":/images/logo_96.svg"));
        t_widget->setPage(QString("页面%1").arg(page + 1));
        t_widget->setMinimumSize(QSize(250, 150));

        QListWidgetItem *item = new QListWidgetItem(m_pBookMarkListWidget);
        item->setFlags(Qt::ItemIsSelectable);
        item->setSizeHint(QSize(250, 150));

        m_pBookMarkListWidget->insertItem(page, item);
        m_pBookMarkListWidget->setItemWidget(item, t_widget);
    }
}

void BookMarkWidget::dltItem()
{
    if (m_iCurrentIndex >= 0) {
        QListWidgetItem *t_item = m_pBookMarkListWidget->takeItem(m_iCurrentIndex);
        delete t_item;
        t_item = nullptr;
        m_iCurrentIndex = -1;
    }
}

int BookMarkWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (MSG_BOOKMARK_DLTITEM == msgType) {
        //dltItem();
        qDebug() << "dlt bookmark item";
        return ConstantMsg::g_effective_res;
    }

    return 0;
}

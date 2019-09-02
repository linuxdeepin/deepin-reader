#include "ThumbnailWidget.h"
#include "ThumbnailItemWidget.h"
/*
 *
 */
ThumbnailWidget::ThumbnailWidget(CustomWidget *parent) :
    CustomWidget(parent)
{
    m_pvBoxLayout = new QVBoxLayout;
    m_pvBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pvBoxLayout->setSpacing(0);
    this->setLayout(m_pvBoxLayout);

    initWidget();

    connect(m_pThumbnailListWidget, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotShowSelectItem(QListWidgetItem *)));
}

int ThumbnailWidget::dealWithData(const int &msgType, const QString &msgContant)
{
    if (MSG_THUMBNAIL_JUMPTOPAGE == msgType) {
        int row = msgContant.toInt();

        setCurrentRow(row);

        return ConstantMsg::g_effective_res;
    }

    return 0;
}

void ThumbnailWidget::initWidget()
{
    m_pThumbnailListWidget = new DListWidget;
    m_pThumbnailListWidget->setSpacing(10);
    //设置自动适应布局调整（Adjust适应，Fixed不适应），默认不适应
    m_pThumbnailListWidget->setResizeMode(QListWidget::Adjust);
    m_pvBoxLayout->addWidget(m_pThumbnailListWidget);
    //m_pThumbnailListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //m_pThumbnailListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_pPageWidget = new PagingWidget;
    m_pvBoxLayout->addWidget(m_pPageWidget);

    for (int idex = 0; idex < 30; ++idex) {
        ThumbnailItemWidget *widget = new ThumbnailItemWidget;
        widget->setContantLabelPixmap(QString(":/images/logo_96.svg"));
        widget->setPageLabelText(QString("               %1").arg(idex + 1));
        widget->setMinimumSize(QSize(250, 250));

        QListWidgetItem *item = new QListWidgetItem(m_pThumbnailListWidget);
        //item->setBackgroundColor(Qt::GlobalColor::lightGray);
        //item->setBackgroundColor(Qt::GlobalColor::white);
        item->setFlags(Qt::NoItemFlags);
        item->setFlags(Qt::ItemIsSelectable);
        item->setSizeHint(QSize(250, 250));

        m_pThumbnailListWidget->insertItem(idex, item);
        //m_pThumbnailListWidget->addItem(item);
        m_pThumbnailListWidget->setItemWidget(item, widget);
    }
}

void ThumbnailWidget::setSelectItemBackColor(QListWidgetItem *item)
{
    if (m_pThumbnailItemWidget) {
        m_pThumbnailItemWidget->setPaint(false);
        m_pSonWidgetContantLabel->setFrameShape (QFrame::NoFrame);

        //qDebug() << "clean background color";
    }
    if (item) {
        DWidget *t_widget = nullptr;
        DLabel *t_pageLab = nullptr;
        DLabel *t_contantLab = nullptr;
        ThumbnailItemWidget *t_ItemWidget = (ThumbnailItemWidget *)(m_pThumbnailListWidget->itemWidget(item));
        t_ItemWidget->setPaint(true);
        t_widget = t_ItemWidget->getSonWidget();
        t_widget->setAutoFillBackground(true);
//        QPalette p;
//        p.setColor(QPalette::Background, QColor(126, 192, 238));
//        t_widget->setPalette(p);
        //t_widget->setBackgroundColor(Qt::GlobalColor::white);
        t_contantLab = t_ItemWidget->getContantLabel();
        t_contantLab->setFrameShape (QFrame::Box);
        //t_contantLab->setPalette(p);
        t_contantLab->setLineWidth(2);

        t_pageLab = t_ItemWidget->getContantLabel();

        qDebug() << "set background color";
        m_pSonWidgetContantLabel = t_contantLab;
        m_pSonWidgetPageLabel = t_pageLab;
        m_pThumbnailItemWidget = t_ItemWidget;
    }
}

void ThumbnailWidget::setCurrentRow(const int &row)
{
    m_pThumbnailListWidget->setCurrentRow(row);
    QListWidgetItem *item = m_pThumbnailListWidget->item(row);
    setSelectItemBackColor(item);
    //slotShowSelectItem(item);
}

void ThumbnailWidget::slotShowSelectItem(QListWidgetItem *item)
{
    setSelectItemBackColor(item);

    int row = m_pThumbnailListWidget->row(item);

    sendMsg(MSG_THUMBNAIL_JUMPTOPAGE, QString::number(row));

    if (m_pPageWidget) {
        m_pPageWidget->setCurrentPageValue(row);
    }
}

void ThumbnailWidget::slotSetJumpToPage(const int &index)
{
    QListWidgetItem *item = m_pThumbnailListWidget->item(index);
    setSelectItemBackColor(item);
}

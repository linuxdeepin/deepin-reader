#include "ThumbnailItemWidget.h"
#include <QDebug>

ThumbnailItemWidget::ThumbnailItemWidget(CustomItemWidget *parent) :
    CustomItemWidget ("ThumbnailItemWidget", parent)
{
    setWindowFlags (Qt::FramelessWindowHint);
    initWidget();
}

void ThumbnailItemWidget::paintEvent(QPaintEvent *event)
{
    QPalette p;
    if (m_bPaint) {
        p.setColor(QPalette::Text, QColor(30, 144, 255));
    } else {
        p.setColor(QPalette::Text, QColor(0, 0, 0));
    }

    m_pPicture->setPalette(p);
    m_pPage->setPalette(p);
    CustomWidget::paintEvent(event);
}

int ThumbnailItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void ThumbnailItemWidget::setBSelect(const bool &paint)
{
    m_bPaint = paint;
}

void ThumbnailItemWidget::initWidget()
{
    m_pPicture = new CustomLabel(this);
    m_pPage = new DLabel();

    m_pPage->setFixedSize(QSize(120, 30));
    m_pPicture->setFixedSize(QSize(120, 150));
    m_pPicture->setAlignment(Qt::AlignCenter);
    m_pPage->setAlignment(Qt::AlignCenter);

    auto t_vLayout = new QVBoxLayout;
    auto t_hLayout = new QHBoxLayout;

    t_vLayout->addWidget(m_pPicture);
    t_vLayout->addWidget(m_pPage);

    t_hLayout->setContentsMargins(1, 0, 1, 0);
    t_hLayout->addSpacing(1);
    t_hLayout->addItem(t_vLayout);
    t_hLayout->addSpacing(1);

    this->setLayout(t_hLayout);
}

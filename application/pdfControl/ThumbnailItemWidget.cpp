#include "ThumbnailItemWidget.h"
#include <QDebug>

ThumbnailItemWidget::ThumbnailItemWidget(CustomWidget *parent) :
    CustomWidget ("ThumbnailItemWidget", parent)
{
    initWidget();
}

int ThumbnailItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void ThumbnailItemWidget::setContantLabelPixmap(const QImage &image)
{
    m_pContantLabel->setPixmap(QPixmap::fromImage(image));
}

void ThumbnailItemWidget::setPageLabelText(const QString &text)
{
    m_pPageLabel->setText(text);
}

void ThumbnailItemWidget::initWidget()
{
    m_pContantLabel = new CustomLabel(this);
    m_pPageLabel = new DLabel();

    m_pPageLabel->setFixedSize(QSize(120, 30));
    m_pContantLabel->setFixedSize(QSize(120, 150));
    m_pContantLabel->setAlignment(Qt::AlignCenter);
    m_pPageLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *t_vLayout = new QVBoxLayout;
    QHBoxLayout *t_hLayout = new QHBoxLayout;

    t_vLayout->addWidget(m_pContantLabel);
    t_vLayout->addWidget(m_pPageLabel);

    t_hLayout->setContentsMargins(1, 0, 1, 0);
    t_hLayout->addSpacing(1);
    t_hLayout->addItem(t_vLayout);
    t_hLayout->addSpacing(1);

    this->setLayout(t_hLayout);
}

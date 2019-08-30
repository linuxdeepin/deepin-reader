#include "ThumbnailItemWidget.h"

ThumbnailItemWidget::ThumbnailItemWidget()
{
    m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

    initWidget();
}

void ThumbnailItemWidget::setContantLabelPixmap(const QString &pix)
{
    m_pContantLabel->setPixmap(QPixmap(pix));
}

void ThumbnailItemWidget::setPageLabelText(const QString &text)
{
    QFont ft;
    ft.setPointSize(12);
    m_pPageLabel->setFont(ft);
    m_pPageLabel->setText(text);
}

void ThumbnailItemWidget::initWidget()
{
    QGridLayout * gridLayout = new QGridLayout;
    m_sonWidget = new DWidget;
    m_sonWidget->setFixedSize(QSize(150,150));
    m_sonWidget->setAutoFillBackground(true);
    m_sonWidget->setLayout(gridLayout);

    m_pContantLabel = new DLabel(this);
    m_pPageLabel = new DLabel(this);
    m_pPageLabel->setFixedSize(QSize(250, 20));

    m_pContantLabel->setFixedSize(QSize(150,150));
    gridLayout->addWidget(m_pContantLabel);

    m_pVLayout->addWidget(m_sonWidget);
    m_pVLayout->addWidget(m_pPageLabel);
    this->setLayout(m_pVLayout);
}

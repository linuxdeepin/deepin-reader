#include "BufferWidget.h"

BufferWidget::BufferWidget(CustomWidget *parent)
    : CustomWidget(QString("BufferWidget"), parent)
{
    initWidget();
}

int BufferWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void BufferWidget::initWidget()
{
    auto m_pVLayout = new QVBoxLayout(this);  // 承载spinner的垂直布局
    auto m_pHLayout = new QHBoxLayout(this);  // 承载spinner的水平布局
    auto m_pSpinner = new DSpinner(this);     // 缓冲动画

    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pHLayout->setContentsMargins(0, 0, 0, 0);

    m_pSpinner->setFixedSize(50, 50);
    m_pSpinner->start();

    m_pHLayout->addStretch(1);
    m_pHLayout->addWidget(m_pSpinner);
    m_pHLayout->addStretch(1);

    m_pVLayout->addStretch(1);
    m_pVLayout->addItem(m_pHLayout);
    m_pVLayout->addStretch(1);

    this->setLayout(m_pVLayout);
}

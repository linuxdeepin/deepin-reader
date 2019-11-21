#include "ThumbnailItemWidget.h"

ThumbnailItemWidget::ThumbnailItemWidget(CustomItemWidget *parent) :
    CustomItemWidget ("ThumbnailItemWidget", parent)
{
    setWindowFlags (Qt::FramelessWindowHint);
    initWidget();
}

// 处理消息接口
int ThumbnailItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

// 是否被选中，选中就就填充颜色
void ThumbnailItemWidget::setBSelect(const bool &paint)
{
    if (m_pPicture) {
        m_pPicture->setSelect(paint);
    }

    if (m_pPageNumber) {
        m_pPageNumber->setSelect(paint);
    }
}

// 初始化界面
void ThumbnailItemWidget::initWidget()
{
    m_pPageNumber = new PageNumberLabel();
    m_pPageNumber->setFixedSize(QSize(146, 18));
    m_pPageNumber->setAlignment(Qt::AlignCenter);

    m_pPicture = new ImageLabel();
    m_pPicture->setFixedSize(QSize(146, 174));
    m_pPicture->setAlignment(Qt::AlignCenter);
    m_pPicture->setRadius(10);

    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(1, 0, 1, 0);
    t_vLayout->setSpacing(0);

    t_vLayout->addWidget(m_pPicture);
    t_vLayout->addWidget(m_pPageNumber);

    auto t_hLayout = new QHBoxLayout;
    t_hLayout->setContentsMargins(2, 0, 2, 0);
    t_hLayout->setSpacing(0);
    t_hLayout->addStretch(1);
    t_hLayout->addItem(t_vLayout);
    t_hLayout->addStretch(1);

    this->setLayout(t_hLayout);
}

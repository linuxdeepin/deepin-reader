#include "ThumbnailItemWidget.h"

ThumbnailItemWidget::ThumbnailItemWidget(CustomItemWidget *parent) :
    CustomItemWidget ("ThumbnailItemWidget", parent)
{
    setWindowFlags (Qt::FramelessWindowHint);
    initWidget();
}

// 绘制  给label填充颜色
void ThumbnailItemWidget::paintEvent(QPaintEvent *event)
{
    QPalette p;
    QPalette pPage;
    QColor color;

    //  涉及到 主题颜色
    if (m_bPaint) {
        color = QColor(QString("#0081FF"));
        p.setColor(QPalette::Text, color);
        pPage.setColor(QPalette::Text, color);
    } else {
        color = QColor::fromRgbF(0, 0, 0, 0.08);
        p.setColor(QPalette::Text, color);
        pPage.setColor(QPalette::Text, QColor(QString("#303030")));
    }

    m_pPicture->setPalette(p);
    m_pPageNumber->setPalette(pPage);
    CustomWidget::paintEvent(event);
}

// 处理消息接口
int ThumbnailItemWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

// 是否被选中，选中就就填充颜色
void ThumbnailItemWidget::setBSelect(const bool &paint)
{
    m_bPaint = paint;
    update();
}

// 初始化界面
void ThumbnailItemWidget::initWidget()
{
    m_pPageNumber = new DLabel();
    m_pPageNumber->setFixedSize(QSize(140, 30));
    m_pPageNumber->setAlignment(Qt::AlignCenter);

    m_pPicture = new CustomLabel();
    m_pPicture->setFixedSize(QSize(140, 168));
    m_pPicture->setAlignment(Qt::AlignCenter);

    auto t_vLayout = new QVBoxLayout;
    t_vLayout->setContentsMargins(1, 0, 1, 0);
    t_vLayout->setSpacing(1);

    t_vLayout->addWidget(m_pPicture);
    t_vLayout->addWidget(m_pPageNumber);

    auto t_hLayout = new QHBoxLayout;
    t_hLayout->setContentsMargins(2, 2, 2, 2);
    t_hLayout->setSpacing(0);
    t_hLayout->addStretch(1);
    t_hLayout->addItem(t_vLayout);
    t_hLayout->addStretch(1);

    this->setLayout(t_hLayout);
}

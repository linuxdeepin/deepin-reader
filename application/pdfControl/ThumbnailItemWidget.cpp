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

void ThumbnailItemWidget::setContantLabelPixmap(const QString &pix)
{
    m_pContantLabel->setPixmap(QPixmap(pix));
}

void ThumbnailItemWidget::setPageLabelText(const QString &text)
{
    QFont ft;
    ft.setPointSize(12);
    m_pPageLabel->setFont(ft);
    m_pPageLabel->setAlignment(Qt::AlignHCenter);
    m_pPageLabel->setAlignment(Qt::AlignVCenter);
    m_pPageLabel->setText(text);
}

void ThumbnailItemWidget::paintEvent(QPaintEvent *event)
{
    //Q_UNUSED(event);
//    m_pContantLabel->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
//    m_pContantLabel->setMainWindowFlags(Qt::FramelessMainWindowHint); //设置无边框窗口

//    QPainter painter(m_pContantLabel);
//    painter.setRenderHint(QPainter::Antialiasing);  // 反锯齿;
//    painter.setBrush(QBrush(Qt::yellow));
//    painter.setPen(Qt::transparent);
//    QRect rect = m_pContantLabel->rect();
//    rect.setWidth(rect.width() - 1);
//    rect.setHeight(rect.height() - 1);
//    painter.drawRoundedRect(rect, 20, 20);

    DWidget::paintEvent(event);
}

void ThumbnailItemWidget::initWidget()
{
    m_pContantLabel = new MyLabel(this);
    m_pPageLabel = new DLabel();

    m_pPageLabel->setFixedSize(QSize(200, 20));
    m_pContantLabel->setFixedSize(QSize(150, 150));

    QVBoxLayout *t_vLayout = new QVBoxLayout;
    QHBoxLayout *t_hLayout = new QHBoxLayout;

    t_vLayout->addWidget(m_pContantLabel);
    t_vLayout->addWidget(m_pPageLabel);

    t_hLayout->addSpacing(1);
    t_hLayout->addItem(t_vLayout);
    t_hLayout->addSpacing(1);

    this->setLayout(t_hLayout);
}

#include "ThumbnailItemWidget.h"
#include <QDebug>

ThumbnailItemWidget::ThumbnailItemWidget(CustomWidget *parent) :
    CustomWidget (parent)
{
    m_pVLayout = new QVBoxLayout;
    m_pVLayout->setContentsMargins(0, 0, 0, 0);
    m_pVLayout->setSpacing(0);
    this->setLayout(m_pVLayout);

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
    m_pPageLabel->setText(text);
}

void ThumbnailItemWidget::paintEvent(QPaintEvent *event)
{
    //Q_UNUSED(event);
//    m_pContantLabel->setAttribute(Qt::WA_TranslucentBackground);//设置窗口背景透明
//    m_pContantLabel->setWindowFlags(Qt::FramelessWindowHint); //设置无边框窗口

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
    QGridLayout *gridLayout = new QGridLayout;
    m_sonWidget = new DWidget;
    m_sonWidget->setFixedSize(QSize(200, 200));
    //m_sonWidget->setAutoFillBackground(true);
    m_sonWidget->setLayout(gridLayout);

    m_pContantLabel = new DLabel();
    m_pPageLabel = new DLabel();
    m_pPageLabel->setFixedSize(QSize(200, 20));

    m_pContantLabel->setFixedSize(QSize(150, 150));
    gridLayout->setContentsMargins(5, 5, 5, 5);
    gridLayout->addWidget(m_pContantLabel);

    m_pVLayout->addWidget(m_sonWidget);
    m_pVLayout->addWidget(m_pPageLabel);
    this->setLayout(m_pVLayout);
}

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
    Q_UNUSED(event);

//    QPainter pw(m_sonWidget?m_sonWidget:nullptr);
//    pw.setPen(Qt::NoPen);
//    pw.setBrush(m_bPaint?Qt::white:Qt::darkBlue);
//    pw.drawRect(rect());

//    m_sonWidget->dealWithData();
//    qDebug() << "paint ground color";

//    QPainter pl(m_pPageLabel?m_pPageLabel:nullptr);
//    pl.setPen(Qt::NoPen);
//    pl.setBrush(m_bPaint?Qt::white:Qt::blue);
//    pl.drawRect(rect());
}

void ThumbnailItemWidget::initWidget()
{
    QGridLayout *gridLayout = new QGridLayout;
    m_sonWidget = new DWidget;
    m_sonWidget->setFixedSize(QSize(250, 250));
    m_sonWidget->setAutoFillBackground(true);
    m_sonWidget->setLayout(gridLayout);

    m_pContantLabel = new DLabel();
    m_pPageLabel = new DLabel();
    m_pPageLabel->setFixedSize(QSize(250, 20));

    m_pContantLabel->setFixedSize(QSize(150, 150));
    gridLayout->setContentsMargins(5, 5, 5, 5);
    gridLayout->addWidget(m_pContantLabel);

    m_pVLayout->addWidget(m_sonWidget);
    m_pVLayout->addWidget(m_pPageLabel);
    this->setLayout(m_pVLayout);
}

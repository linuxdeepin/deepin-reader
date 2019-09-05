#include "fontWidget.h"
#include <QDebug>

FontWidget::FontWidget(CustomWidget *parent):
    CustomWidget("FontWidget", parent)
{
    setWindowFlags(Qt::Popup);



    initWidget();
}

FontWidget::~FontWidget()
{

}

int FontWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void FontWidget::initWidget()
{
    //让程序无边框
    setWindowFlags( Qt::FramelessWindowHint );
    //让程序背景透明
    setAttribute(Qt::WA_TranslucentBackground, true);

    QFont ft;
    QVBoxLayout *t_pVBoxLayout = new QVBoxLayout;
    t_pVBoxLayout->setContentsMargins(1, 0, 1, 0);
    t_pVBoxLayout->setSpacing(0);

    QHBoxLayout *t_pHLayout1 = new QHBoxLayout;
    t_pHLayout1->setContentsMargins(0, 0, 0, 0);
    t_pHLayout1->setSpacing(0);
    QHBoxLayout *t_pHLayout2 = new QHBoxLayout;
    t_pHLayout2->setContentsMargins(0, 0, 0, 0);
    t_pHLayout2->setSpacing(0);
    QHBoxLayout *t_pHLayout3 = new QHBoxLayout;
    t_pHLayout3->setContentsMargins(0, 0, 0, 0);
    t_pHLayout3->setSpacing(0);
    QHBoxLayout *t_pHLayout4 = new QHBoxLayout;
    t_pHLayout4->setContentsMargins(0, 0, 0, 0);
    t_pHLayout4->setSpacing(0);
    QHBoxLayout *t_pHLayout5 = new QHBoxLayout;
    t_pHLayout5->setContentsMargins(0, 0, 0, 0);
    t_pHLayout5->setSpacing(0);
    QHBoxLayout *t_pHLayout6 = new QHBoxLayout;
    t_pHLayout6->setContentsMargins(0, 0, 0, 0);
    t_pHLayout6->setSpacing(0);
    QHBoxLayout *t_pHLayout7 = new QHBoxLayout;
    t_pHLayout6->setContentsMargins(0, 0, 0, 0);
    t_pHLayout6->setSpacing(0);

    m_pEnlargeLab = new QLabel;
    m_pEnlargeLab->setText(tr("200%"));
    m_pEnlargeLab->setAlignment(Qt::AlignCenter);
    ft.setPointSize(15);
    m_pEnlargeLab->setFont(ft);
    m_pEnlargeLab->setFixedSize(QSize(150, 25));

    //t_pHLayout1->setSpacing(1);
    t_pHLayout1->addWidget(m_pEnlargeLab);
    //t_pHLayout1->setSpacing(1);

    m_pMinLabALab = new QLabel;
    m_pMinLabALab->setText(tr("A"));
    m_pMinLabALab->setAlignment(Qt::AlignCenter);
    ft.setPointSize(8);
    m_pMinLabALab->setFont(ft);
    m_pMinLabALab->setFixedSize(QSize(25, 25));

    m_pMaxLabALab = new QLabel;
    m_pMaxLabALab->setText(tr("A"));
    m_pMaxLabALab->setAlignment(Qt::AlignCenter);
    ft.setPointSize(12);
    m_pMaxLabALab->setFont(ft);
    m_pMaxLabALab->setFixedSize(QSize(30, 25));

    m_pEnlargeSlider = new DSlider(Qt::Horizontal);
    m_pEnlargeSlider->setMinimum(50);
    m_pEnlargeSlider->setMaximum(300);
    m_pEnlargeSlider->setValue(200);
    m_pEnlargeSlider->setTickPosition(QSlider::TicksBothSides);
    //m_pEnlargeSlider->setLayoutDirection(Qt::LeftToRight);
    m_pEnlargeSlider->setFixedSize(QSize(100, 25));
    connect(m_pEnlargeSlider, SIGNAL(valueChanged(int)),  SLOT(slotSetChangeVal(int)));

    //t_pHLayout2->addSpacing(1);
    t_pHLayout2->addWidget(m_pMinLabALab);
    t_pHLayout2->addWidget(m_pEnlargeSlider);
    t_pHLayout2->addWidget(m_pMaxLabALab);
    //t_pHLayout2->addSpacing(1);

    m_pDoubPageViewBtn = new DImageButton;
    m_pDoubPageViewBtn->setText(tr("    双页视图"));
    m_pDoubPageViewBtn->setAlignment(Qt::AlignCenter);
    ft.setPointSize(11);
    m_pDoubPageViewBtn->setFont(ft);
    m_pDoubPageViewBtn->setFixedSize(QSize(120, 25));
    m_pDoubPageViewLab = new QLabel;
    m_pDoubPageViewLab->setFixedSize(QSize(30, 25));
    connect(m_pDoubPageViewBtn, SIGNAL(clicked()), SLOT(slotSetDoubPageViewCheckIcon()));
    //t_pHLayout3->addSpacing(1);
    t_pHLayout3->addWidget(m_pDoubPageViewBtn);
    t_pHLayout3->addWidget(m_pDoubPageViewLab);
    //t_pHLayout3->addStretch(1);

    m_pSuitHBtn = new DImageButton;
    m_pSuitHBtn->setText(tr("    适合高度"));
    m_pSuitHBtn->setAlignment(Qt::AlignCenter);
    ft.setPointSize(12);
    m_pSuitHBtn->setFont(ft);
    m_pSuitHBtn->setFixedSize(QSize(120, 25));
    connect(m_pSuitHBtn, SIGNAL(clicked()), SLOT(slotSetSuitHCheckIcon()));
    m_pSuitHLab = new QLabel;
    m_pSuitHLab->setFixedSize(QSize(30, 25));
    //t_pHLayout4->addSpacing(1);
    t_pHLayout4->addWidget(m_pSuitHBtn);
    t_pHLayout4->addWidget(m_pSuitHLab);
    //t_pHLayout4->addSpacing(1);

    m_pSuitWBtn = new DImageButton;
    m_pSuitWBtn->setText(tr("    适合宽度"));
    m_pSuitWBtn->setAlignment(Qt::AlignCenter);
    ft.setPointSize(12);
    m_pSuitWBtn->setFont(ft);
    m_pSuitWBtn->setFixedSize(QSize(120, 25));
    connect(m_pSuitWBtn, SIGNAL(clicked()), SLOT(slotSetSuitWCheckIcon()));
    m_pSuitWLab = new QLabel;
    m_pSuitWLab->setFixedSize(QSize(30, 25));
    //t_pHLayout5->addSpacing(1);
    t_pHLayout5->addWidget(m_pSuitWBtn);
    t_pHLayout5->addWidget(m_pSuitWLab);
    //t_pHLayout5->addSpacing(1);

    m_pRotateLeftBtn = new DImageButton;
    m_pRotateLeftBtn->setText(tr("左旋转"));
    m_pRotateLeftBtn->setAlignment(Qt::AlignCenter);
    ft.setPointSize(12);
    m_pRotateLeftBtn->setFont(ft);
    m_pRotateLeftBtn->setFixedSize(QSize(120, 25));
    connect(m_pRotateLeftBtn, SIGNAL(clicked()), SLOT(slotSetRotateLeftCheckIcon()));
    m_pRotateLeftLab = new QLabel;
    m_pRotateLeftLab->setFixedSize(QSize(30, 25));
    //t_pHLayout6->addSpacing(1);
    t_pHLayout6->addWidget(m_pRotateLeftBtn);
    t_pHLayout6->addWidget(m_pRotateLeftLab);
    //t_pHLayout6->addSpacing(1);

    m_pRotateRightBtn = new DImageButton;
    m_pRotateRightBtn->setText(tr("右旋转"));
    m_pRotateRightBtn->setAlignment(Qt::AlignCenter);
    ft.setPointSize(12);
    m_pRotateRightBtn->setFont(ft);
    m_pRotateRightBtn->setFixedSize(QSize(120, 25));
    connect(m_pRotateRightBtn, SIGNAL(clicked()), SLOT(slotSetRotateRightCheckIcon()));
    m_pRotateRightLab = new QLabel;
    m_pRotateRightLab->setFixedSize(QSize(30, 25));
    //t_pHLayout7->addSpacing(1);
    t_pHLayout7->addWidget(m_pRotateRightBtn);
    t_pHLayout7->addWidget(m_pRotateRightLab);
    //t_pHLayout7->addSpacing(1);


    t_pVBoxLayout->setContentsMargins(0, 0, 1, 0);
    t_pVBoxLayout->addSpacing(1);
    t_pVBoxLayout->addItem(t_pHLayout1);
    t_pVBoxLayout->addItem(t_pHLayout2);
    t_pVBoxLayout->addItem(t_pHLayout3);
    t_pVBoxLayout->addItem(t_pHLayout4);
    t_pVBoxLayout->addItem(t_pHLayout5);
    t_pVBoxLayout->addItem(t_pHLayout6);
    t_pVBoxLayout->addItem(t_pHLayout7);
    this->setLayout(t_pVBoxLayout);
}

void FontWidget::paintEvent(QPaintEvent *)
{
    QRectF rectangle(14.0, 12.0, this->width() - 30, this->height() - 20);

    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawRoundedRect(rectangle, 14, 12);

    const int w = this->width() / 2;

    QPointF points[3] = {
        QPointF(10 + w - 10, 0),
        QPointF(10 + w - 15, 12),
        QPointF(10 + w - 5, 12),
    };

    QPen pen;
    pen.setColor(QColor(240, 240, 240));
    painter.setPen(pen);
    painter.drawPolygon(points, 3);

    QPen pen1;
    pen1.setColor(Qt::black);
    painter.setPen(pen1);
    painter.drawLine(points[0], points[1]);
    painter.drawLine(points[0], points[2]);

    QPointF points1[] = {
        QPointF(15, 76),
        QPointF(w * 2 - 17, 76),

        QPointF(40, 108),
        QPointF(w * 2 - 17, 108),

        QPointF(40, 140),
        QPointF(w * 2 - 17, 140),

        QPointF(15, 175),
        QPointF(w * 2 - 17, 175),

        QPointF(40, 208),
        QPointF(w * 2 - 17, 208),
    };

    QPen pen2;
    pen2.setColor(Qt::gray);
    pen2.setWidth(2);
    painter.setPen(pen2);
    painter.drawLine(points1[0], points1[1]);
    painter.drawLine(points1[6], points1[7]);

    pen2.setColor(Qt::gray);
    pen2.setWidth(1);
    painter.setPen(pen2);
    painter.drawLine(points1[2], points1[3]);
    painter.drawLine(points1[4], points1[5]);
    painter.drawLine(points1[8], points1[9]);

    this->update();
}

void FontWidget::slotSetChangeVal(int val)
{
    m_pEnlargeLab->clear();
    m_pEnlargeLab->setText(tr("%1%").arg(val));

    sendMsg(MSG_SCALEUP_MULTIPLE, QString::number(val));
}

void FontWidget::slotSetDoubPageViewCheckIcon()
{
    static bool t_isDoubPage = false;

    qDebug() << tr("slotSetDoubPageViewCheckIcon");

    t_isDoubPage = !t_isDoubPage;

    if (t_isDoubPage) {
        m_pDoubPageViewLab->setPixmap(QPixmap(tr(":/image/checked/bookmark.svg")));
        sendMsg(MSG_DOUB_PAGE_VIEW, QString::number(1));
    } else {
        m_pDoubPageViewLab->setPixmap(QPixmap(tr("")));
    }
}

void FontWidget::slotSetSuitHCheckIcon()
{
    static bool t_isSuitH = false;

    t_isSuitH = !t_isSuitH;

    if (t_isSuitH) {
        m_pSuitHLab->setPixmap(QPixmap(tr(":/image/checked/bookmark.svg")));
        sendMsg(MSG_SELF_ADAPTE_HEIGHT, QString::number(1));
    } else {
        m_pSuitHLab->setPixmap(QPixmap(tr("")));
    }
}

void FontWidget::slotSetSuitWCheckIcon()
{
    static bool t_isSuitW = false;

    t_isSuitW = !t_isSuitW;

    if (t_isSuitW) {
        m_pSuitWLab->setPixmap(QPixmap(tr(":/image/checked/bookmark.svg")));
        sendMsg(MSG_SELF_ADAPTE_WIDTH, QString::number(1));
    } else {
        m_pSuitWLab->setPixmap(QPixmap(tr("")));
    }
}

void FontWidget::slotSetRotateLeftCheckIcon()
{
    static bool t_isRotateL = false;

    t_isRotateL = !t_isRotateL;

    if (t_isRotateL) {
        m_pRotateLeftLab->setPixmap(QPixmap(tr(":/image/checked/bookmark.svg")));
        sendMsg(MSG_ROTATE_LEFT, QString::number(1));
    } else {
        m_pRotateLeftLab->setPixmap(QPixmap(tr("")));
    }
}

void FontWidget::slotSetRotateRightCheckIcon()
{
    static bool t_isRotateR = false;

    t_isRotateR = !t_isRotateR;

    if (t_isRotateR) {
        m_pRotateRightLab->setPixmap(QPixmap(tr(":/image/checked/bookmark.svg")));
        sendMsg(MSG_ROTATE_RIGHT, QString::number(1));
    } else {
        m_pRotateRightLab->setPixmap(QPixmap(tr("")));
    }
}


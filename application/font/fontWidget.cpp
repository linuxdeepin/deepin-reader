#include "fontWidget.h"
#include <QDebug>

FontWidget::FontWidget(CustomWidget *parent):
    CustomWidget("FontWidget", parent)
{
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
    m_pEnlargeLab->setFixedSize(QSize(50, 50));

    t_pHLayout1->setSpacing(1);
    t_pHLayout1->addWidget(m_pEnlargeLab);
    t_pHLayout1->setSpacing(1);

    m_pMinLabALab = new QLabel;
    m_pMinLabALab->setText(tr("A"));
    m_pMinLabALab->setAlignment(Qt::AlignCenter);
    ft.setPointSize(10);
    m_pMinLabALab->setFont(ft);
    m_pMinLabALab->setFixedSize(QSize(50, 50));

    m_pMaxLabALab = new QLabel;
    m_pMaxLabALab->setText(tr("A"));
    m_pMaxLabALab->setAlignment(Qt::AlignCenter);
    ft.setPointSize(12);
    m_pMaxLabALab->setFont(ft);
    m_pMaxLabALab->setFixedSize(QSize(50, 50));

    m_pEnlargeSlider = new DSlider(Qt::Horizontal);
    m_pEnlargeSlider->setMinimum(50);
    m_pEnlargeSlider->setMaximum(300);
    m_pEnlargeSlider->setValue(200);
    m_pEnlargeSlider->setTickPosition(QSlider::TicksBothSides);
    m_pEnlargeSlider->setLayoutDirection(Qt::LeftToRight);
    m_pEnlargeSlider->setFixedSize(QSize(330, 50));
    connect(m_pEnlargeSlider, SIGNAL(valuChanged(int)), SLOT(slotSetChangeVal(int)));

    t_pHLayout2->addSpacing(1);
    t_pHLayout2->addWidget(m_pMinLabALab);
    t_pHLayout2->addWidget(m_pEnlargeSlider);
    t_pHLayout2->addWidget(m_pMaxLabALab);
    t_pHLayout2->addSpacing(1);

    m_pDoubPageViewBtn = new DImageButton;
    m_pDoubPageViewBtn->setText(tr("双页视图"));
    m_pDoubPageViewBtn->setAlignment(Qt::AlignLeft);
    m_pDoubPageViewBtn->setFont(ft);
    m_pDoubPageViewBtn->setFixedSize(QSize(350, 50));
    m_pDoubPageViewLab = new QLabel;
    m_pDoubPageViewLab->setFixedSize(QSize(50, 50));
    connect(m_pDoubPageViewBtn, SIGNAL(clicked()), SLOT(slotSetDoubPageViewCheckIcon()));
    t_pHLayout3->addSpacing(1);
    t_pHLayout3->addWidget(m_pDoubPageViewBtn);
    t_pHLayout3->addWidget(m_pDoubPageViewLab);
    t_pHLayout3->addSpacing(1);

    m_pSuitHBtn = new DImageButton;
    m_pSuitHBtn->setText(tr("适合高度"));
    m_pSuitHBtn->setAlignment(Qt::AlignLeft);
    m_pSuitHBtn->setFont(ft);
    m_pSuitHBtn->setFixedSize(QSize(350, 50));
    connect(m_pSuitHBtn, SIGNAL(clicked()), SLOT(slotSetSuitHCheckIcon()));
    m_pSuitHLab = new QLabel;
    m_pSuitHLab->setFixedSize(QSize(50, 50));
    t_pHLayout4->addSpacing(1);
    t_pHLayout4->addWidget(m_pSuitHBtn);
    t_pHLayout4->addWidget(m_pSuitHLab);
    t_pHLayout4->addSpacing(1);

    m_pSuitWBtn = new DImageButton;
    m_pSuitWBtn->setText(tr("适合宽度"));
    m_pSuitWBtn->setAlignment(Qt::AlignLeft);
    m_pSuitWBtn->setFont(ft);
    m_pSuitWBtn->setFixedSize(QSize(350, 50));
    connect(m_pSuitWBtn, SIGNAL(clicked()), SLOT(slotSetSuitWCheckIcon()));
    m_pSuitWLab = new QLabel;
    m_pSuitWLab->setFixedSize(QSize(50, 50));
    t_pHLayout5->addSpacing(1);
    t_pHLayout5->addWidget(m_pSuitWBtn);
    t_pHLayout5->addWidget(m_pSuitWLab);
    t_pHLayout5->addSpacing(1);

    m_pRotateLeftBtn = new DImageButton;
    m_pRotateLeftBtn->setText(tr("左旋转"));
    m_pRotateLeftBtn->setAlignment(Qt::AlignLeft);
    m_pRotateLeftBtn->setFont(ft);
    m_pRotateLeftBtn->setFixedSize(QSize(350, 50));
    connect(m_pRotateLeftBtn, SIGNAL(clicked()), SLOT(slotSetRotateLeftCheckIcon()));
    m_pRotateLeftLab = new QLabel;
    m_pRotateLeftLab->setFixedSize(QSize(50, 50));
    t_pHLayout6->addSpacing(1);
    t_pHLayout6->addWidget(m_pRotateLeftBtn);
    t_pHLayout6->addWidget(m_pRotateLeftLab);
    t_pHLayout6->addSpacing(1);

    m_pRotateRightBtn = new DImageButton;
    m_pRotateRightBtn->setText(tr("右旋转"));
    m_pRotateRightBtn->setAlignment(Qt::AlignLeft);
    m_pRotateRightBtn->setFont(ft);
    m_pRotateRightBtn->setFixedSize(QSize(350, 50));
    connect(m_pRotateRightBtn, SIGNAL(clicked()), SLOT(slotSetRotateRightCheckIcon()));
    m_pRotateRightLab = new QLabel;
    m_pRotateRightLab->setFixedSize(QSize(350, 50));
    t_pHLayout7->addSpacing(1);
    t_pHLayout7->addWidget(m_pRotateRightBtn);
    t_pHLayout7->addWidget(m_pRotateRightLab);
    t_pHLayout7->addSpacing(1);

    t_pVBoxLayout->addItem(t_pHLayout1);
    t_pVBoxLayout->addItem(t_pHLayout2);
    t_pVBoxLayout->addItem(t_pHLayout3);
    t_pVBoxLayout->addItem(t_pHLayout4);
    t_pVBoxLayout->addItem(t_pHLayout5);
    t_pVBoxLayout->addItem(t_pHLayout6);
    t_pVBoxLayout->addItem(t_pHLayout7);

    this->setLayout(t_pVBoxLayout);
}

void FontWidget::slotSetChangeVal(int val)
{
    QFont ft;
    ft.setPointSize(15);
    m_pEnlargeLab->setText(tr("%1%").arg(val));
    m_pEnlargeLab->setFont(ft);

    qDebug() << val;
}

void FontWidget::slotSetDoubPageViewCheckIcon()
{
    static bool t_isChecked = false;

    t_isChecked = !t_isChecked;

    if (t_isChecked) {
        m_pDoubPageViewLab->setPixmap(QPixmap(tr(":/image/light/hover/choose_small.svg")));
    } else {
        m_pDoubPageViewLab->setPixmap(QPixmap(tr("")));
    }
}

void FontWidget::slotSetSuitHCheckIcon()
{
    static bool t_isChecked = false;

    t_isChecked = !t_isChecked;

    if (t_isChecked) {
        m_pSuitHLab->setPixmap(QPixmap(tr(":/image/light/hover/choose_small.svg")));
    } else {
        m_pSuitHLab->setPixmap(QPixmap(tr("")));
    }
}

void FontWidget::slotSetSuitWCheckIcon()
{
    static bool t_isChecked = false;

    t_isChecked = !t_isChecked;

    if (t_isChecked) {
        m_pSuitWLab->setPixmap(QPixmap(tr(":/image/light/hover/choose_small.svg")));
    } else {
        m_pSuitWLab->setPixmap(QPixmap(tr("")));
    }
}

void FontWidget::slotSetRotateLeftCheckIcon()
{
    static bool t_isChecked = false;

    t_isChecked = !t_isChecked;

    if (t_isChecked) {
        m_pRotateLeftLab->setPixmap(QPixmap(tr(":/image/light/hover/choose_small.svg")));
    } else {
        m_pRotateLeftLab->setPixmap(QPixmap(tr("")));
    }
}

void FontWidget::slotSetRotateRightCheckIcon()
{
    static bool t_isChecked = false;

    t_isChecked = !t_isChecked;

    if (t_isChecked) {
        m_pRotateRightLab->setPixmap(QPixmap(tr(":/image/light/hover/choose_small.svg")));
    } else {
        m_pRotateRightLab->setPixmap(QPixmap(tr("")));
    }
}


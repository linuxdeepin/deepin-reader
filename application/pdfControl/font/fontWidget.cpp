#include "fontWidget.h"
#include "translator/PdfControl.h"

FontWidget::FontWidget(CustomWidget *parent):
    CustomWidget(QString("FontWidget"), parent)
{
    //让程序无边框
    setWindowFlags(Qt::Popup);

    initWidget();
}

int FontWidget::dealWithData(const int &msgType, const QString &)
{
    int scale = 0;
    switch (msgType) {
    case MSG_OPERATION_LARGER:      //  放大
        scale = m_pEnlargeSlider->value();
        if (scale < 300) {
            scale += 25;
            if (scale > 300) {
                scale = 300;
            }
            m_pEnlargeSlider->setValue(scale);
        }
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SMALLER:     //  缩小
        scale = m_pEnlargeSlider->value();
        if (scale > 50) {
            scale -= 25;
            if (scale <= 50) {
                scale = 50;
            }
            m_pEnlargeSlider->setValue(scale);
        }
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

void FontWidget::initWidget()
{
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

    m_pEnlargeLab = new DLabel;
    m_pEnlargeLab->setText(QString("100%"));
    m_pEnlargeLab->setAlignment(Qt::AlignCenter);
    ft.setPointSize(15);
    m_pEnlargeLab->setFont(ft);
    m_pEnlargeLab->setFixedSize(QSize(150, 25));

    t_pHLayout1->setSpacing(1);
    t_pHLayout1->addWidget(m_pEnlargeLab);
    t_pHLayout1->setSpacing(1);

//    QStringList t_list;
//    t_list << QString("0.5") << QString("0.75") << QString("1.0") << QString("1.25") << QString("1.5") << QString("1.75") << QString("2.0")
//           << QString("2.25") << QString("2.5") << QString("2.75") << QString("3.0");

    m_pEnlargeSlider = new DSlider(Qt::Horizontal);
    m_pEnlargeSlider->setMinimum(50);
    m_pEnlargeSlider->setMaximum(300);
    m_pEnlargeSlider->setValue(100);
    m_pEnlargeSlider->slider()->setSingleStep(25);
    m_pEnlargeSlider->setPageStep(25);
    m_pEnlargeSlider->slider()->setTickPosition(QSlider::TicksBelow);
    m_pEnlargeSlider->setFixedSize(QSize(175, 25));
    m_pEnlargeSlider->setLeftIcon(QIcon(tr(":/resources/image/A_small.svg")));
    m_pEnlargeSlider->setRightIcon(QIcon(tr(":/resources/image/A_big.svg")));
//    m_pEnlargeSlider->setLeftTicks(t_list);
    connect(m_pEnlargeSlider, SIGNAL(valueChanged(int)), this, SLOT(slotSetChangeVal(int)));

    t_pHLayout2->addSpacing(1);
    t_pHLayout2->addWidget(m_pEnlargeSlider);
    t_pHLayout2->addSpacing(1);

    m_pDoubPageViewLb = new MenuLab(this);
    m_pDoubPageViewLb->setText(PdfControl::DOUB_VIEW);
    m_pDoubPageViewLb->setAlignment(Qt::AlignLeft);
    ft.setPointSize(12);
    m_pDoubPageViewLb->setFont(ft);
    m_pDoubPageViewLb->setFixedSize(QSize(120, 25));

    m_pDoubPageViewLab = new DLabel;
    m_pDoubPageViewLab->setFixedSize(QSize(30, 25));
    connect(m_pDoubPageViewLb, SIGNAL(clicked()), SLOT(slotSetDoubPageViewCheckIcon()));
    t_pHLayout3->addWidget(m_pDoubPageViewLb);
    t_pHLayout3->addWidget(m_pDoubPageViewLab);

    m_pSuitHLb = new MenuLab(this);
    m_pSuitHLb->setText(PdfControl::ADAP_HEIGHT);
    m_pSuitHLb->setAlignment(Qt::AlignLeft);
    ft.setPointSize(12);
    m_pSuitHLb->setFont(ft);
    m_pSuitHLb->setFixedSize(QSize(120, 25));
    connect(m_pSuitHLb, SIGNAL(clicked()), SLOT(slotSetSuitHCheckIcon()));
    m_pSuitHLab = new DLabel;
    m_pSuitHLab->setFixedSize(QSize(30, 25));
    t_pHLayout4->addWidget(m_pSuitHLb);
    t_pHLayout4->addWidget(m_pSuitHLab);

    m_pSuitWLb = new MenuLab(this);
    m_pSuitWLb->setText(PdfControl::ADAP_WIDTH);
    m_pSuitWLb->setAlignment(Qt::AlignLeft);
    ft.setPointSize(12);
    m_pSuitWLb->setFont(ft);
    m_pSuitWLb->setFixedSize(QSize(120, 25));
    connect(m_pSuitWLb, SIGNAL(clicked()), SLOT(slotSetSuitWCheckIcon()));
    m_pSuitWLab = new DLabel;
    m_pSuitWLab->setFixedSize(QSize(30, 25));
    t_pHLayout5->addWidget(m_pSuitWLb);
    t_pHLayout5->addWidget(m_pSuitWLab);

    m_pRotateLeftLb = new MenuLab(this);
    m_pRotateLeftLb->setText(PdfControl::ROTAT_TO_L);
    m_pRotateLeftLb->setAlignment(Qt::AlignLeft);
    ft.setPointSize(12);
    m_pRotateLeftLb->setFont(ft);
    m_pRotateLeftLb->setFixedSize(QSize(140, 25));
    connect(m_pRotateLeftLb, SIGNAL(clicked()), SLOT(slotSetRotateLeftCheckIcon()));
    m_pRotateLeftLab = new DLabel;
    m_pRotateLeftLab->setFixedSize(QSize(30, 25));
    t_pHLayout6->addWidget(m_pRotateLeftLb);
    t_pHLayout6->addWidget(m_pRotateLeftLab);

    m_pRotateRightLb = new MenuLab(this);
    m_pRotateRightLb->setText(PdfControl::ROTAT_TO_R);
    m_pRotateRightLb->setAlignment(Qt::AlignLeft);
    ft.setPointSize(12);
    m_pRotateRightLb->setFont(ft);
    m_pRotateRightLb->setFixedSize(QSize(140, 25));
    connect(m_pRotateRightLb, SIGNAL(clicked()), SLOT(slotSetRotateRightCheckIcon()));
    m_pRotateRightLab = new DLabel;
    m_pRotateRightLab->setFixedSize(QSize(30, 25));
    t_pHLayout7->addWidget(m_pRotateRightLb);
    t_pHLayout7->addWidget(m_pRotateRightLab);

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
    QRectF rectangle(0.0, 12.0, this->width(), this->height() - 12);

    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawRoundedRect(rectangle, 14, 12);

    const int w = this->width() / 2;

    QPointF points[3] = {
        QPointF(10 + w - 10, 0),
        QPointF(10 + w - 18, 12),
        QPointF(10 + w - 2, 12),
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
        QPointF(0, 76),
        QPointF(w * 2, 76),

        QPointF(20, 108),
        QPointF(w * 2, 108),

        QPointF(20, 140),
        QPointF(w * 2, 140),

        QPointF(0, 175),
        QPointF(w * 2, 175),

        QPointF(20, 208),
        QPointF(w * 2, 208),
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

void FontWidget::hideEvent(QHideEvent *event)
{
    emit sigWidgetHide();
    CustomWidget::hideEvent(event);
}

void FontWidget::rotateFileView(bool isRight)
{
    int ival = m_pEnlargeSlider->value();

    if (isRight) {
        m_rotate += 90;
    } else {
        m_rotate -= 90;
    }

    m_rotate = (m_rotate < 0) ? (m_rotate + 360) : m_rotate;

    scaleAndRotate(ival);
    sendMsg(MSG_FILE_ROTATE, QString(""));
}

void FontWidget::scaleAndRotate(int ival)
{
    int t_rotate = 0;

    t_rotate = (m_rotate / 90) % 4;

    t_rotate = (t_rotate < 0) ? (t_rotate * -1) : t_rotate;

    t_rotate += 1;


    switch (t_rotate) {
    case RotateType_0:
        m_rotate = 0;
        m_rotateType = RotateType_0;
        DocummentProxy::instance()->scaleRotateAndShow((ival * 0.01), RotateType_0);
        break;
    case RotateType_90:
        m_rotateType = RotateType_90;
        DocummentProxy::instance()->scaleRotateAndShow((ival * 0.01), RotateType_90);
        break;
    case RotateType_180:
        m_rotateType = RotateType_180;
        DocummentProxy::instance()->scaleRotateAndShow((ival * 0.01), RotateType_180);
        break;
    case RotateType_270:
        m_rotateType = RotateType_270;
        DocummentProxy::instance()->scaleRotateAndShow((ival * 0.01), RotateType_270);
        break;
    default:
        m_rotateType = RotateType_Normal;
        DocummentProxy::instance()->scaleRotateAndShow((ival * 0.01), RotateType_Normal);
        break;
    }
}

void FontWidget::setShowSuitHIcon()
{
    int t_nShow = m_bSuitH ? 1 : 0;
    sendMsg(MSG_SELF_ADAPTE_HEIGHT, QString::number(t_nShow));

    if (m_bSuitH) {
        m_pSuitHLab->setPixmap(QPixmap(QString(":/resources/image/select.svg")));
    } else {
        m_pSuitHLab->setPixmap(QPixmap(QString("")));
    }
}

void FontWidget::setShowSuitWIcon()
{
    int t_nShow = m_bSuitW ? 1 : 0;
    sendMsg(MSG_SELF_ADAPTE_WIDTH, QString::number(t_nShow));

    if (m_bSuitW) {
        m_pSuitWLab->setPixmap(QPixmap(QString(":/resources/image/select.svg")));
    } else {
        m_pSuitWLab->setPixmap(QPixmap(QString("")));
    }
}

void FontWidget::slotSetChangeVal(int val)
{
    m_pEnlargeLab->clear();
    m_pEnlargeLab->setText(tr("%1%").arg(val));

    scaleAndRotate(val);
}

void FontWidget::slotSetDoubPageViewCheckIcon()
{
    static bool t_isDoubPage = false;

    t_isDoubPage = !t_isDoubPage;

    if (t_isDoubPage) {
        m_pDoubPageViewLab->setPixmap(QPixmap(QString(":/resources/image/select.svg")));
        DocummentProxy::instance()->setViewModeAndShow(ViewMode_FacingPage);
    } else {
        m_pDoubPageViewLab->setPixmap(QPixmap(QString("")));
        DocummentProxy::instance()->setViewModeAndShow(ViewMode_SinglePage);
    }
}

void FontWidget::slotSetSuitHCheckIcon()
{
    m_bSuitW = false;
    m_pSuitWLab->setPixmap(QPixmap(QString("")));

    m_bSuitH = !m_bSuitH;
    setShowSuitHIcon();
}

void FontWidget::slotSetSuitWCheckIcon()
{
    m_bSuitH = false;
    m_pSuitHLab->setPixmap(QPixmap(QString("")));

    m_bSuitW = !m_bSuitW;
    setShowSuitWIcon();
}

void FontWidget::slotSetRotateLeftCheckIcon()
{
    rotateFileView(false);
}

void FontWidget::slotSetRotateRightCheckIcon()
{
    rotateFileView(true);
}


MenuLab::MenuLab(QWidget *parent):
    DLabel(parent)
{

}

void MenuLab::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

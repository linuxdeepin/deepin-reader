#include "fontWidget.h"

FontWidget::FontWidget(CustomWidget *parent):
    CustomWidget("FontWidget", parent)
{
    initWidget();

    initConnection();
}

/**
 * @brief FontWidget::dealWithData
 * 处理全局信号接口
 * @param msgType
 * @return
 */
int FontWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    int scale = 0;
    switch (msgType) {
    case MSG_OPERATION_LARGER:      //  放大
        scale = m_pEnlargeSlider->value();
        if (scale < 500) {
            scale += 25;
            if (scale > 500) {
                scale = 500;
            }
            m_pEnlargeSlider->setValue(scale);
        }
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_SMALLER:     //  缩小
        scale = m_pEnlargeSlider->value();
        if (scale > 10) {
            scale -= 25;
            if (scale <= 10) {
                scale = 10;
            }
            m_pEnlargeSlider->setValue(scale);
        }
        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_OPEN_FILE_OK:
        emit sigOpenFileOk();
        break;
    case MSG_SELF_ADAPTE_SCALE:
        m_bIsAdaptMove = true;
        m_pEnlargeSlider->setValue(msgContent.toDouble() * 100);
        break;
    }

    return 0;
}

/**
 * @brief FontWidget::initWidget
 * 初始化界面
 */
void FontWidget::initWidget()
{
    initScaleLabel();
    initScaleSlider();
    initDowbleShow();

    initAdaptateHeight();
    initAdaptateWidght();

    auto pRotateLeftLb = new MenuLab(this);
    pRotateLeftLb->setText(tr("Rotated To Left"));
    pRotateLeftLb->setAlignment(Qt::AlignLeft);
    connect(pRotateLeftLb, SIGNAL(clicked()), this, SLOT(slotSetRotateLeftCheckIcon()));

    auto pRotateRightLb = new MenuLab(this);
    pRotateRightLb->setText(tr("Rotated To Right"));
    pRotateRightLb->setAlignment(Qt::AlignLeft);
    connect(pRotateRightLb, SIGNAL(clicked()), this, SLOT(slotSetRotateRightCheckIcon()));

    //  垂直布局
    auto widgetLayout = new QVBoxLayout();
    widgetLayout->setContentsMargins(5, 0, 5, 0);
    widgetLayout->setSpacing(3);

    widgetLayout->addWidget(m_pEnlargeLab);

    widgetLayout->addWidget(m_pEnlargeSlider);

    widgetLayout->addWidget(getLabelLineH());

    widgetLayout->addItem(m_pDoubleShowLayout);

    widgetLayout->addWidget(getLabelLineH());

    //  自适应高\宽
    widgetLayout->addItem(m_pAdaptateHeightLayout);
    widgetLayout->addItem(m_pAdaptateWidghtLayout);

    widgetLayout->addWidget(getLabelLineH());

    //  旋转
    widgetLayout->addWidget(pRotateLeftLb);
    widgetLayout->addWidget(pRotateRightLb);

    this->setLayout(widgetLayout);
}

/**
 * @brief FontWidget::slotReset
 * 重新打开文件后，复位个控件状态
 */
void FontWidget::slotReset()
{
    m_pEnlargeLab->setText(QString("100%"));
    m_pEnlargeSlider->setValue(100);

    m_pDoubPageViewLab->setPixmap(QPixmap(QString("")));

    m_bSuitH = false;
    m_bSuitW = false;
    m_pSuitHLab->setPixmap(QPixmap(QString("")));
    m_pSuitWLab->setPixmap(QPixmap(QString("")));

    m_rotate = 0;
    m_rotateType = RotateType_Normal;
}

/**
 * @brief FontWidget::rotateFileView
 *计算旋转函数
 * @param isRight
 */
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

/**
 * @brief FontWidget::scaleAndRotate
 * 右侧页面旋转函数
 * @param ival
 */
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

/**
 * @brief FontWidget::setShowSuitHIcon
 * 自适应高函数
 */
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

/**
 * @brief FontWidget::setShowSuitWIcon
 * 自适应宽函数
 */
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

/**
 * @brief FontWidget::initConnection
 * 初始化信号槽
 */
void FontWidget::initConnection()
{
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotReset()));
}

//  分割线
DLabel *FontWidget::getLabelLineH()
{
    auto labelLine = new DLabel(this);
    labelLine->setFixedHeight(2);
    labelLine->setFrameShape(QFrame::HLine);

    return labelLine;
}

void FontWidget::initScaleLabel()
{
    m_pEnlargeLab = new DLabel;
    m_pEnlargeLab->setText(QString("100%"));
    m_pEnlargeLab->setAlignment(Qt::AlignCenter);
}

void FontWidget::initScaleSlider()
{
    m_pEnlargeSlider = new DSlider(Qt::Horizontal);
    m_pEnlargeSlider->setMinimum(10);
    m_pEnlargeSlider->setMaximum(500);
    m_pEnlargeSlider->setValue(100);
    m_pEnlargeSlider->slider()->setSingleStep(25);
    m_pEnlargeSlider->setPageStep(25);
    m_pEnlargeSlider->slider()->setTickPosition(QSlider::TicksBelow);
    m_pEnlargeSlider->setLeftIcon(QIcon(":/resources/image/A_small.svg"));
    m_pEnlargeSlider->setRightIcon(QIcon(":/resources/image/A_big.svg"));
    connect(m_pEnlargeSlider, SIGNAL(valueChanged(int)), this, SLOT(slotSetChangeVal(int)));

}

void FontWidget::initDowbleShow()
{
    m_pDoubleShowLayout = new QHBoxLayout;
    m_pDoubleShowLayout->setContentsMargins(0, 0, 0, 0);
    m_pDoubleShowLayout->setSpacing(0);

    auto m_pDoubPageViewLb = new MenuLab(this);
    m_pDoubPageViewLb->setText(tr("Double View"));
    m_pDoubPageViewLb->setAlignment(Qt::AlignLeft);
    connect(m_pDoubPageViewLb, SIGNAL(clicked()), this, SLOT(slotSetDoubPageViewCheckIcon()));
    m_pDoubleShowLayout->addWidget(m_pDoubPageViewLb);

    m_pDoubPageViewLab = new MenuLab(this);
    m_pDoubPageViewLab->setFixedSize(QSize(30, 25));
    connect(m_pDoubPageViewLab, SIGNAL(clicked()), this, SLOT(slotSetDoubPageViewCheckIcon()));
    m_pDoubleShowLayout->addWidget(m_pDoubPageViewLab);
}

void FontWidget::initAdaptateHeight()
{
    m_pAdaptateHeightLayout = new QHBoxLayout;
    m_pAdaptateHeightLayout->setContentsMargins(0, 0, 0, 0);
    m_pAdaptateHeightLayout->setSpacing(0);

    auto pSuitHLb = new MenuLab(this);
    pSuitHLb->setText(tr("Adaptate Height"));
    pSuitHLb->setAlignment(Qt::AlignLeft);
    pSuitHLb->setFixedSize(QSize(120, 25));
    connect(pSuitHLb, SIGNAL(clicked()), this, SLOT(slotSetSuitHCheckIcon()));
    m_pAdaptateHeightLayout->addWidget(pSuitHLb);

    m_pSuitHLab = new MenuLab(this);
    m_pSuitHLab->setFixedSize(QSize(30, 25));
    connect(m_pSuitHLab, SIGNAL(clicked()), this, SLOT(slotSetSuitHCheckIcon()));
    m_pAdaptateHeightLayout->addWidget(m_pSuitHLab);
}

void FontWidget::initAdaptateWidght()
{
    m_pAdaptateWidghtLayout = new QHBoxLayout;
    m_pAdaptateWidghtLayout->setContentsMargins(0, 0, 0, 0);
    m_pAdaptateWidghtLayout->setSpacing(0);

    auto pSuitWLb = new MenuLab(this);
    pSuitWLb->setText(tr("Adaptate Width"));
    pSuitWLb->setAlignment(Qt::AlignLeft);
    pSuitWLb->setFixedSize(QSize(120, 25));
    connect(pSuitWLb, SIGNAL(clicked()), this, SLOT(slotSetSuitWCheckIcon()));
    m_pAdaptateWidghtLayout->addWidget(pSuitWLb);

    m_pSuitWLab = new MenuLab(this);
    m_pSuitWLab->setFixedSize(QSize(30, 25));
    connect(m_pSuitWLab, SIGNAL(clicked()), this, SLOT(slotSetSuitWCheckIcon()));

    m_pAdaptateWidghtLayout->addWidget(m_pSuitWLab);
}


/**
 * @brief FontWidget::slotSetChangeVal
 * 设置字体缩放比例函数
 * @param val
 */
void FontWidget::slotSetChangeVal(int val)
{
    m_pEnlargeLab->clear();
    m_pEnlargeLab->setText(QString("%1%").arg(val));

    if (!m_bIsAdaptMove) {
        scaleAndRotate(val);

        m_bSuitW = false;
        m_pSuitWLab->setPixmap(QPixmap(QString("")));

        m_bSuitH = false;
        m_pSuitHLab->setPixmap(QPixmap(QString("")));
    }

    m_bIsAdaptMove = false;
}

/**
 * @brief FontWidget::slotSetDoubPageViewCheckIcon
 * 单/双页显示函数
 */
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

/**
 * @brief FontWidget::slotSetSuitHCheckIcon
 * 设置自适应高度选中图标
 */
void FontWidget::slotSetSuitHCheckIcon()
{
    m_bSuitW = false;
    m_pSuitWLab->setPixmap(QPixmap(QString("")));

    m_bSuitH = !m_bSuitH;
    setShowSuitHIcon();
}

/**
 * @brief FontWidget::slotSetSuitWCheckIcon
 * 设置自适应宽度选中图标
 */
void FontWidget::slotSetSuitWCheckIcon()
{
    m_bSuitH = false;
    m_pSuitHLab->setPixmap(QPixmap(QString("")));

    m_bSuitW = !m_bSuitW;
    setShowSuitWIcon();
}

/**
 * @brief FontWidget::slotSetRotateLeftCheckIcon
 * 向左旋转函数
 */
void FontWidget::slotSetRotateLeftCheckIcon()
{
    rotateFileView(false);
}

/**
 * @brief FontWidget::slotSetRotateRightCheckIcon
 * 向右旋转函数
 */
void FontWidget::slotSetRotateRightCheckIcon()
{
    rotateFileView(true);
}


/*********************自定义label**************************************/
/*********************MenuLab*****××××********************************/

MenuLab::MenuLab(QWidget *parent):
    DLabel(parent)
{

}

/**
 * @brief MenuLab::mousePressEvent
 * 自定义label点击事件
 * @param event
 */
void MenuLab::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
}

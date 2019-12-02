#include "fontWidget.h"
#include "utils/PublicFunction.h"
#include "frame/DocummentFileHelper.h"
#include "utils/utils.h"
#include <DHorizontalLine>

FontWidget::FontWidget(CustomWidget *parent):
    CustomWidget("FontWidget", parent)
{
    shortKeyList = QStringList() << KeyStr::g_ctrl_1 << KeyStr::g_ctrl_2
                   << KeyStr::g_ctrl_3 << KeyStr::g_ctrl_r << KeyStr::g_ctrl_shift_r
                   << KeyStr::g_ctrl_larger << KeyStr::g_ctrl_smaller;
    initWidget();
    initConnection();

    slotUpdateTheme();
}

/**
 * @brief FontWidget::dealWithData
 * 处理全局信号接口
 * @param msgType
 * @return
 */
int FontWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    switch (msgType) {
//    case MSG_OPERATION_LARGER:      //  放大
//        emit sigKeyLargerOrSmaller(1);
//        return ConstantMsg::g_effective_res;
//    case MSG_OPERATION_SMALLER:     //  缩小
//        emit sigKeyLargerOrSmaller(0);
//        return ConstantMsg::g_effective_res;
    case MSG_OPERATION_OPEN_FILE_OK:
        emit sigOpenFileOk();
        break;
    case MSG_SELF_ADAPTE_SCALE:
        emit sigSetCurScale(msgContent);
        break;
    case MSG_OPERATION_UPDATE_THEME:
        emit sigUpdateTheme();
        break;
    case MSG_NOTIFY_KEY_MSG : {
        if (shortKeyList.contains(msgContent)) {
            emit sigDealWithKey(msgContent);
            return ConstantMsg::g_effective_res;
        }
        break;
    }
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

    //  垂直布局
    auto widgetLayout = new QVBoxLayout();
    widgetLayout->setContentsMargins(0, 0, 0, 0);
    // widgetLayout->setSpacing(3);

    widgetLayout->addWidget(m_pEnlargeLab);
    widgetLayout->addItem(m_pEnlargeSliderLayout);
    widgetLayout->addWidget(new DHorizontalLine);//getLabelLineH(4));

    widgetLayout->addItem(m_pDoubleShowLayout);
    widgetLayout->addWidget(new DHorizontalLine);//getLabelLineH());

    //  自适应高\宽
    widgetLayout->addItem(m_pAdaptateHeightLayout);
    widgetLayout->addWidget(new DHorizontalLine);//getLabelLineH());
    widgetLayout->addItem(m_pAdaptateWidghtLayout);

    widgetLayout->addWidget(new DHorizontalLine);//getLabelLineH());
    //左旋转
    QHBoxLayout *lrlayout = new QHBoxLayout;
    lrlayout->setContentsMargins(0, 0, 0, 0);
    lrlayout->addSpacing(32);
    auto pRotateLeftLb = new CustomClickLabel(tr("Rotated To Left"), this);
    pRotateLeftLb->setFixedSize(QSize(188, 28));
    lrlayout->addWidget(pRotateLeftLb);
    widgetLayout->addItem(lrlayout);
    connect(pRotateLeftLb, SIGNAL(clicked()), this, SLOT(slotSetRotateLeftCheckIcon()));
    widgetLayout->addWidget(new DHorizontalLine);//getLabelLineH(3));

    //右旋转
    QHBoxLayout *rrlayout = new QHBoxLayout;
    rrlayout->setContentsMargins(0, 0, 0, 0);
    rrlayout->addSpacing(32);
    auto pRotateRightLb = new CustomClickLabel(tr("Rotated To Right"), this);
    pRotateRightLb->setFixedSize(QSize(188, 28));
    rrlayout->addWidget(pRotateRightLb);
    widgetLayout->addItem(rrlayout);
    connect(pRotateRightLb, SIGNAL(clicked()), this, SLOT(slotSetRotateRightCheckIcon()));
    widgetLayout->addStretch();
    this->setLayout(widgetLayout);
}

//  缩放, iFlag > 0, 放大, 否则 缩放
void FontWidget::setFileLargerOrSmaller(const int &iFlag)
{
    if (iFlag > 0) {
        int scale = m_pEnlargeSlider->value();
        if (scale < 500) {
            scale += 25;
            if (scale > 500) {
                scale = 500;
            }
            m_pEnlargeSlider->setValue(scale);
        }
    } else {
        int scale = m_pEnlargeSlider->value();
        if (scale > 10) {
            scale -= 25;
            if (scale <= 10) {
                scale = 10;
            }
            m_pEnlargeSlider->setValue(scale);
        }
    }
}

//  快捷键处理
void FontWidget::slotDealWithKey(const QString &sKey)
{
    if (sKey == KeyStr::g_ctrl_1) {
        slotReset();
    } else if (sKey == KeyStr::g_ctrl_2) {
        slotSetSuitHCheckIcon();
    } else if (sKey == KeyStr::g_ctrl_3) {
        slotSetSuitWCheckIcon();
    } else if (sKey == KeyStr::g_ctrl_r) {
        slotSetRotateLeftCheckIcon();
    } else if (sKey == KeyStr::g_ctrl_shift_r) {
        slotSetRotateRightCheckIcon();
    } else if (sKey == KeyStr::g_ctrl_smaller) {
        setFileLargerOrSmaller(0);
    } else if (sKey == KeyStr::g_ctrl_larger) {
        setFileLargerOrSmaller(1);
    }
}

//  主题变了
void FontWidget::slotUpdateTheme()
{
    QString sPixmap = PF::getImagePath("select", Pri::g_pdfControl);
    QPixmap px = Utils::renderSVG(sPixmap, QSize(28, 28));
    m_pSuitHLabelIcon->setPixmap(px);
    m_pSuitWLabelIcon->setPixmap(px);
    m_pDoubPageViewLabelIcon->setPixmap(px);

    QString sSmall = PF::getImagePath("A_small", Pri::g_pdfControl);
    m_pEnlargeSlider->setLeftIcon(QIcon(sSmall));

    QString sBig = PF::getImagePath("A_big", Pri::g_pdfControl);
    m_pEnlargeSlider->setRightIcon(QIcon(sBig));
}

/**
 * @brief FontWidget::slotReset
 * 重新打开文件后，复位个控件状态
 */
void FontWidget::slotReset()
{
    m_pEnlargeLab->setText(QString("100%"));
    m_pEnlargeSlider->setValue(100);

    m_pDoubPageViewLabelIcon->setVisible(false);

    m_bSuitH = false;
    m_bSuitW = false;

    m_pSuitHLabelIcon->setVisible(false);
    m_pSuitWLabelIcon->setVisible(false);

    m_rotate = 0;
    m_rotateType = RotateType_0;

    m_isDoubPage = false;

    setScaleRotateViewModeAndShow();
}

void FontWidget::slotSetCurScale(const QString &sData)
{
    m_bIsAdaptMove = true;
    m_pEnlargeSlider->setValue(sData.toDouble() * 100);
}

/**
 * @brief FontWidget::rotateFileView
 *计算旋转函数
 * @param isRight
 */
void FontWidget::rotateFileView(bool isRight)
{
    if (isRight) {
        m_rotate += 90;
    } else {
        m_rotate -= 90;
    }

    m_rotate = (m_rotate < 0) ? (m_rotate + 360) : m_rotate;

    scaleAndRotate();

    notifyMsg(MSG_FILE_ROTATE, QString(""));
}

/**
 * @brief FontWidget::scaleAndRotate
 * 右侧页面旋转函数
 * @param ival
 */
void FontWidget::scaleAndRotate()
{
    int t_rotate = 0;

    t_rotate = (m_rotate / 90) % 4;

    t_rotate = (t_rotate < 0) ? (t_rotate * -1) : t_rotate;

    t_rotate += 1;

    switch (t_rotate) {
    case RotateType_0:
        m_rotate = 0;
        m_rotateType = RotateType_0;
        break;
    case RotateType_90:
        m_rotateType = RotateType_90;
        break;
    case RotateType_180:
        m_rotateType = RotateType_180;
        break;
    case RotateType_270:
        m_rotateType = RotateType_270;
        break;
    default:
        m_rotateType = RotateType_Normal;
        break;
    }

    setScaleRotateViewModeAndShow();
}

/**
 * @brief FontWidget::setShowSuitHIcon
 * 自适应高函数
 */
void FontWidget::setShowSuitHIcon()
{
    m_pSuitWLabelIcon->setVisible(false);
    m_pSuitHLabelIcon->setVisible(m_bSuitH);

    int t_nShow = m_bSuitH ? 1 : 0;
    notifyMsg(MSG_SELF_ADAPTE_HEIGHT, QString::number(t_nShow));
}

/**
 * @brief FontWidget::setShowSuitWIcon
 * 自适应宽函数
 */
void FontWidget::setShowSuitWIcon()
{
    m_pSuitWLabelIcon->setVisible(m_bSuitW);
    m_pSuitHLabelIcon->setVisible(false);

    int t_nShow = m_bSuitW ? 1 : 0;
    notifyMsg(MSG_SELF_ADAPTE_WIDTH, QString::number(t_nShow));
}

/**
 * @brief FontWidget::initConnection
 * 初始化信号槽
 */
void FontWidget::initConnection()
{
//    connect(this, SIGNAL(sigKeyLargerOrSmaller(const int &)), SLOT(slotKeyLargerOrSmaller(const int &)));
    connect(this, SIGNAL(sigUpdateTheme()), SLOT(slotUpdateTheme()));
    connect(this, SIGNAL(sigOpenFileOk()), SLOT(slotReset()));
    connect(this, SIGNAL(sigSetCurScale(const QString &)), SLOT(slotSetCurScale(const QString &)));
    connect(this, SIGNAL(sigDealWithKey(const QString &)), SLOT(slotDealWithKey(const QString &)));
}

//  分割线
DLabel *FontWidget::getLabelLineH(int fixheight)
{
    auto labelLine = new DLabel(this);
    labelLine->setFixedHeight(fixheight);
    labelLine->setFrameShape(QFrame::HLine);

    return labelLine;
}

void FontWidget::initScaleLabel()
{
    m_pEnlargeLab = new CustomClickLabel("100%");
    m_pEnlargeLab->setAlignment(Qt::AlignCenter);
}

void FontWidget::initScaleSlider()
{
    m_pEnlargeSliderLayout = new QHBoxLayout;
    m_pEnlargeSlider = new DSlider(Qt::Horizontal);
    m_pEnlargeSliderLayout->addSpacing(24);
    m_pEnlargeSliderLayout->addWidget(m_pEnlargeSlider);
    m_pEnlargeSliderLayout->addSpacing(21);
    m_pEnlargeSlider->setMinimum(10);
    m_pEnlargeSlider->setMaximum(500);
    m_pEnlargeSlider->setValue(100);
    m_pEnlargeSlider->slider()->setSingleStep(25);
    m_pEnlargeSlider->setPageStep(25);
    m_pEnlargeSlider->slider()->setTickPosition(QSlider::TicksBelow);

    connect(m_pEnlargeSlider, SIGNAL(valueChanged(int)), this, SLOT(slotSetChangeVal(int)));
}

void FontWidget::initDowbleShow()
{
    m_pDoubleShowLayout = new QHBoxLayout;
    m_pDoubleShowLayout->addSpacing(32);
    m_pDoubleShowLayout->setContentsMargins(0, 0, 10, 0);
    m_pDoubleShowLayout->setSpacing(0);

    auto pDoubPageViewLabel = new CustomClickLabel(tr("Double View"), this);
    pDoubPageViewLabel->setFixedSize(QSize(140, 28));
    connect(pDoubPageViewLabel, SIGNAL(clicked()), this, SLOT(slotSetDoubPageViewCheckIcon()));
    m_pDoubleShowLayout->addWidget(pDoubPageViewLabel);

    m_pDoubleShowLayout->addStretch();

    m_pDoubPageViewLabelIcon = new CustomClickLabel("", this);
    m_pDoubPageViewLabelIcon->hide();
    m_pDoubPageViewLabelIcon->setFixedSize(QSize(28, 28));
    connect(m_pDoubPageViewLabelIcon, SIGNAL(clicked()), this, SLOT(slotSetDoubPageViewCheckIcon()));
    m_pDoubleShowLayout->addWidget(m_pDoubPageViewLabelIcon);
}

void FontWidget::initAdaptateHeight()
{
    m_pAdaptateHeightLayout = new QHBoxLayout;
    m_pAdaptateHeightLayout->setContentsMargins(0, 0, 10, 0);
    m_pAdaptateHeightLayout->addSpacing(32);
    m_pAdaptateHeightLayout->setSpacing(0);

    auto pSuitHLb = new CustomClickLabel(tr("Adaptate Height"), this);
    pSuitHLb->setFixedSize(QSize(150, 28));
    connect(pSuitHLb, SIGNAL(clicked()), this, SLOT(slotSetSuitHCheckIcon()));
    m_pAdaptateHeightLayout->addWidget(pSuitHLb);

    m_pAdaptateHeightLayout->addStretch();

    m_pSuitHLabelIcon = new CustomClickLabel("", this);
    m_pSuitHLabelIcon->hide();
    m_pSuitHLabelIcon->setFixedSize(QSize(28, 28));
    connect(m_pSuitHLabelIcon, SIGNAL(clicked()), this, SLOT(slotSetSuitHCheckIcon()));
    m_pAdaptateHeightLayout->addWidget(m_pSuitHLabelIcon);
}

void FontWidget::initAdaptateWidght()
{
    m_pAdaptateWidghtLayout = new QHBoxLayout;
    m_pAdaptateWidghtLayout->setContentsMargins(0, 0, 10, 0);
    m_pAdaptateWidghtLayout->addSpacing(32);
    m_pAdaptateWidghtLayout->setSpacing(0);

    auto pSuitWLb = new CustomClickLabel(tr("Adaptate Width"), this);
    pSuitWLb->setFixedSize(QSize(150, 28));
    connect(pSuitWLb, SIGNAL(clicked()), this, SLOT(slotSetSuitWCheckIcon()));
    m_pAdaptateWidghtLayout->addWidget(pSuitWLb);

    m_pAdaptateWidghtLayout->addStretch(1);

    m_pSuitWLabelIcon = new CustomClickLabel("", this);
    m_pSuitWLabelIcon->hide();
    m_pSuitWLabelIcon->setFixedSize(QSize(28, 28));
    connect(m_pSuitWLabelIcon, SIGNAL(clicked()), this, SLOT(slotSetSuitWCheckIcon()));

    m_pAdaptateWidghtLayout->addWidget(m_pSuitWLabelIcon);

}

void FontWidget::setScaleRotateViewModeAndShow()
{
    int ival = m_pEnlargeSlider->value();
    double scale = (ival * 0.01);
    ViewMode_EM viewmode = ViewMode_SinglePage;
    if (m_isDoubPage) {
        viewmode = ViewMode_FacingPage;
    }
    DocummentFileHelper::instance()->setScaleRotateViewModeAndShow(scale, m_rotateType, viewmode);
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
        scaleAndRotate();

        m_bSuitW = false;
        m_bSuitH = false;

        m_pSuitHLabelIcon->setVisible(false);
        m_pSuitWLabelIcon->setVisible(false);
    }

    m_bIsAdaptMove = false;
}

/**
 * @brief FontWidget::slotSetDoubPageViewCheckIcon
 * 单/双页显示函数
 */
void FontWidget::slotSetDoubPageViewCheckIcon()
{
    m_isDoubPage = !m_isDoubPage;
    m_pDoubPageViewLabelIcon->setVisible(m_isDoubPage);

    setScaleRotateViewModeAndShow();
}

/**
 * @brief FontWidget::slotSetSuitHCheckIcon
 * 设置自适应高度选中图标
 */
void FontWidget::slotSetSuitHCheckIcon()
{
    m_bSuitW = false;
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

#include "AppAboutWidget.h"
#include <DWidgetUtil>

AppAboutWidget::AppAboutWidget(CustomWidget *parent)
    : CustomWidget ("AppAboutWidget", parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setFixedSize(QSize(720, 560));
    setAttribute(Qt::WA_ShowModal, true); //  模态对话框， 属性设置

    m_pVBoxLayout = new QVBoxLayout;
    m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
    m_pVBoxLayout->setSpacing(10);
    this->setLayout(m_pVBoxLayout);

    initWidget();
}

int AppAboutWidget::dealWithData(const int &, const QString &)
{
    return 0;
}

void AppAboutWidget::initWidget()
{
    m_pVBoxLayout->addStretch(1);
    initCloseBtn();
    initLabels();
    m_pVBoxLayout->addStretch(1);
}

void AppAboutWidget::showScreenCenter()
{
    Dtk::Widget::moveToCenter(this);
    this->show();
}

void AppAboutWidget::initCloseBtn()
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addStretch(1);

    DPushButton *closeBtn = new DPushButton("", this);
    closeBtn->setToolTip(tr("close"));
    connect(closeBtn, &DPushButton::clicked, this, &AppAboutWidget::slotBtnCloseClicked);

    layout->addWidget(closeBtn);

    m_pVBoxLayout->addItem(layout);
}

void AppAboutWidget::initLabels()
{
    DLabel *logoLabel = new DLabel(this);
    logoLabel->setAlignment(Qt::AlignCenter);
    logoLabel->setPixmap(QPixmap(":/resources/image/logo/logo_big.svg"));

    DLabel *appNameLabel = new DLabel(tr("deep_in_reader"), this);
    appNameLabel->setAlignment(Qt::AlignCenter);

    DLabel *appVersionLabel = new DLabel(tr("Version: 1.0.0"), this);
    appVersionLabel->setAlignment(Qt::AlignCenter);

    DLabel *tipLabel = new DLabel(tr("www.deepin.org"), this);
    tipLabel->setAlignment(Qt::AlignCenter);

    DLabel *thankLabel = new DLabel(tr("thanks"), this);
    thankLabel->setAlignment(Qt::AlignCenter);

    DLabel *introduceLabel = new DLabel(tr("文档查看器是深度操作系统自带的文档查看软件。\r\n"
                                           "除了打开和阅读PDF文件外，你还可以对文档进行\r\n"
                                           "添加书签、添加注释和对选择的文本进行高亮显示等操作。"), this);
    introduceLabel->setAlignment(Qt::AlignCenter);

    DLabel *deepinLabel = new DLabel("", this);
    deepinLabel->setAlignment(Qt::AlignCenter);
    deepinLabel->setPixmap(QPixmap(":/resources/deepin_logo.svg"));

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->setSpacing(10);
    vlayout->setContentsMargins(0, 0, 0, 0);

    vlayout->addWidget(logoLabel);
    vlayout->addWidget(appNameLabel);
    vlayout->addWidget(appVersionLabel);
    vlayout->addWidget(deepinLabel);
    vlayout->addWidget(tipLabel);
    vlayout->addWidget(thankLabel);
    vlayout->addWidget(introduceLabel);

    m_pVBoxLayout->addItem(vlayout);
}

void AppAboutWidget::slotBtnCloseClicked()
{
    this->close();
}


void AppAboutWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bClickDown = true;
        //获得鼠标的初始位置
        mouseStartPoint = event->globalPos();
        //mouseStartPoint = event->pos();
        //获得窗口的初始位置
        windowTopLeftPoint = this->frameGeometry().topLeft();
    }
    CustomWidget::mousePressEvent(event);
}

void AppAboutWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_bClickDown) {
        //获得鼠标移动的距离
        QPoint distance = event->globalPos() - mouseStartPoint;
        //QPoint distance = event->pos() - mouseStartPoint;
        //改变窗口的位置
        this->move(windowTopLeftPoint + distance);
    }
    CustomWidget::mouseMoveEvent(event);
}

void AppAboutWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bClickDown = false;
    }
    CustomWidget::mouseReleaseEvent(event);
}

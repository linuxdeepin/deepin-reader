#include "TitleWidget.h"
#include "translator/Frame.h"
#include <QHBoxLayout>

TitleWidget::TitleWidget(CustomWidget *parent) :
    CustomWidget("TitleWidget", parent)
{
    initWidget();
    initConnections();
}

TitleWidget::~TitleWidget()
{
    if (m_pFontWidget) {
        m_pFontWidget->deleteLater();
        m_pFontWidget = nullptr;
    }
}

//  打开文件成功
void TitleWidget::slotOpenFileOk()
{
    m_pThumbnailBtn->setDisabled(false);
    m_pSettingBtn->setDisabled(false);
    m_pHandleShapeBtn->setDisabled(false);
    m_pMagnifierBtn->setDisabled(false);
}

// 应用全屏显示
void TitleWidget::slotAppFullScreen()
{
    m_pThumbnailBtn->setChecked(false);
}

//  退出放大鏡
void TitleWidget::slotMagnifierCancel()
{
    m_pMagnifierBtn->setChecked(false);
    sendMsgToSubject(MSG_MAGNIFYING, QString::number(0));
}

void TitleWidget::initWidget()
{
    m_pFontWidget = new FontWidget;
    connect(m_pFontWidget, SIGNAL(sigWidgetHide()), this, SLOT(slotFontWidgetHide()));

    initBtns();

    auto m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(5, 0, 0, 0);
    m_layout->setSpacing(10);
    setLayout(m_layout);

    m_layout->addWidget(m_pThumbnailBtn);
    m_layout->addWidget(m_pSettingBtn);
    m_layout->addWidget(m_pHandleShapeBtn);
    m_layout->addWidget(m_pMagnifierBtn);

    m_layout->addStretch(1);
}

void TitleWidget::initConnections()
{
    connect(this, SIGNAL(sigOpenFileOk()), this, SLOT(slotOpenFileOk()));
    connect(this, SIGNAL(sigMagnifierCancel()), this, SLOT(slotMagnifierCancel()));
    connect(this, SIGNAL(sigAppFullScreen()), this, SLOT(slotAppFullScreen()));
}

//  缩略图 显示
void TitleWidget::on_thumbnailBtn_clicked()
{
    bool rl = m_pThumbnailBtn->isChecked();
    sendMsgToSubject(MSG_SLIDER_SHOW_STATE, QString::number(rl));
}

//  字体
void TitleWidget::on_settingBtn_clicked()
{
    int nHeight = m_pSettingBtn->height();
    int nWidth = m_pSettingBtn->width() / 2;

    QPoint point = m_pSettingBtn->mapToGlobal(QPoint(0, 0));
    int nOldY = point.y();
    int nOldX = point.x();

    point.setX(nWidth + nOldX - 100);
    point.setY(nHeight + nOldY);

    m_pFontWidget->setGeometry(point.x(), point.y(), 200, 250);
    m_pFontWidget->show();
}

//  手型点击
void TitleWidget::on_handleShapeBtn_clicked()
{
    int nHeight = m_pHandleShapeBtn->height();

    QPoint point = m_pHandleShapeBtn->mapToGlobal(QPoint(0, 0));
    int nOldY = point.y();

    point.setY(nHeight + nOldY + 2);

    if (m_pHandleMenu == nullptr) {
        m_pHandleMenu = new DMenu(this);
        m_pHandleMenu->setFixedWidth(200);
        connect(m_pHandleMenu, SIGNAL(aboutToHide()), this, SLOT(slotHandleMenuHide()));

        QAction *m_pHandleAction = createAction(Frame::sHandleShape);

        QAction *m_pDefaultAction = createAction(Frame::sDefaultShape);
        m_pDefaultAction->setChecked(true);

        QActionGroup *actionGroup = new QActionGroup(this);
        actionGroup->addAction(m_pHandleAction);
        actionGroup->addAction(m_pDefaultAction);

        connect(actionGroup, SIGNAL(triggered(QAction *)), this, SLOT(slotActionTrigger(QAction *)));
    }
    m_pHandleMenu->exec(point);
}

//  放大镜 功能
void TitleWidget::on_magnifyingBtn_clicked()
{
    bool bCheck = m_pMagnifierBtn->isChecked();
    sendMsgToSubject(MSG_MAGNIFYING, QString::number(bCheck));
}

void TitleWidget::slotActionTrigger(QAction *action)
{
    QString btnName = "";
    int nCurrentState = -1;

    QString sAction = action->objectName();
    if (sAction == Frame::sDefaultShape) {
        nCurrentState = 0;
        btnName = Frame::sDefaultShape;
    } else {
        nCurrentState = 1;
        btnName = Frame::sHandleShape;
    }

    setHandleShapeBtn(btnName);
    sendMsgToSubject(MSG_HANDLESHAPE, QString::number(nCurrentState));
}

//  设置　手型按钮的图标
void TitleWidget::setHandleShapeBtn(const QString &btnName)
{
    QString normalPic = PF::getQrcPath(btnName, ImageModule::g_normal_state);

    m_pHandleShapeBtn->setIcon(QIcon(normalPic));
}

void TitleWidget::slotFontWidgetHide()
{
    m_pSettingBtn->setChecked(false);
}

void TitleWidget::slotHandleMenuHide()
{
    m_pHandleShapeBtn->setChecked(false);
}

void TitleWidget::initBtns()
{
    m_pThumbnailBtn = createBtn(Frame::sThumbnail, true);
    connect(m_pThumbnailBtn, SIGNAL(clicked()), this, SLOT(on_thumbnailBtn_clicked()));

    m_pSettingBtn = createBtn(Frame::sSetting);
    connect(m_pSettingBtn, SIGNAL(clicked()), this, SLOT(on_settingBtn_clicked()));

    m_pHandleShapeBtn = createBtn(Frame::sDefaultShape);
    m_pHandleShapeBtn->setFixedSize(QSize(42, 36));
    m_pHandleShapeBtn->setIconSize(QSize(42, 36));
    connect(m_pHandleShapeBtn, SIGNAL(clicked()), this, SLOT(on_handleShapeBtn_clicked()));

    m_pMagnifierBtn = createBtn(Frame::sMagnifier, true);
    connect(m_pMagnifierBtn, SIGNAL(clicked()), this, SLOT(on_magnifyingBtn_clicked()));
}

DIconButton *TitleWidget::createBtn(const QString &btnName, bool bCheckable)
{
    QString normalPic = PF::getQrcPath(btnName, ImageModule::g_normal_state);

    DIconButton *btn = new  DIconButton(this);
    btn->setIcon(QIcon(normalPic));
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));

    btn->setToolTip(btnName);
    btn->setCheckable(bCheckable);
    if (bCheckable) {
        btn->setChecked(false);
    }

    btn->setDisabled(true);

    return btn;
}

//  创建两个按钮菜单， 对应 手型 和 默认鼠标
QAction *TitleWidget::createAction(const QString &actionName)
{
    QAction *_action = new QAction(actionName, this);
    _action->setObjectName(actionName);
    _action->setCheckable(true);
    _action->setIcon(QIcon(QString(":/resources/image/normal/%1_small.svg").arg(actionName)));

    m_pHandleMenu->addAction(_action);
    return _action;
}

void TitleWidget::sendMsgToSubject(const int &msgType, const QString &msgCotent)
{
    sendMsg(msgType, msgCotent);
}

//  处理 推送消息
int TitleWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        emit sigOpenFileOk();
    } else if (msgType == MSG_OPERATION_FULLSCREEN) {
        emit sigAppFullScreen();
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == "Esc") {      //  退出放大镜模式
            emit sigMagnifierCancel();
        }
    }
    return 0;
}

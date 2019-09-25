#include "TitleWidget.h"
#include <QSignalMapper>
#include "translator/Frame.h"

TitleWidget::TitleWidget(CustomWidget *parent) :
    CustomWidget("TitleWidget", parent)
{
    m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(5, 0, 0, 0);
    m_layout->setSpacing(10);
    setLayout(m_layout);

    initWidget();
}

TitleWidget::~TitleWidget()
{
    if (m_pFontWidget) {
        m_pFontWidget->deleteLater();
        m_pFontWidget = nullptr;
    }
}

void TitleWidget::initWidget()
{
    m_pFontWidget = new FontWidget;
    connect(m_pFontWidget, SIGNAL(sigWidgetHide()), this, SLOT(slotFontWidgetHide()));

    initBtns();
    m_layout->addStretch(1);
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
        m_pHandleMenu = new DMenu;
        connect(m_pHandleMenu, SIGNAL(aboutToHide()), this, SLOT(slotHandleMenuHide()));

        QSignalMapper *pSigManager = new QSignalMapper(this);
        connect(pSigManager, SIGNAL(mapped(const QString &)), this, SLOT(SlotActionTrigger(const QString &)));

        m_pHandleAction = createAction(Frame::sHandleShape);
        connect(m_pHandleAction, SIGNAL(triggered()), pSigManager, SLOT(map()));

        pSigManager->setMapping(m_pHandleAction, "HandleAction");

        m_pDefaultAction = createAction(Frame::sDefaultShape);
        m_pDefaultAction->setChecked(true);

        connect(m_pDefaultAction, SIGNAL(triggered()), pSigManager, SLOT(map()));

        pSigManager->setMapping(m_pDefaultAction, "DefaultAction");
    }
    m_pHandleMenu->exec(point);
}

//  放大镜 功能
void TitleWidget::on_magnifyingBtn_clicked()
{
    bool bCheck = m_pMagnifierBtn->isChecked();
    sendMsgToSubject(MSG_MAGNIFYING, QString::number(bCheck));
}

void TitleWidget::SlotActionTrigger(const QString &sAction)
{
    if (sAction == "DefaultAction") {
        on_DefaultAction_trigger();
    } else {
        on_HandleAction_trigger();
    }

    sendMsgToSubject(MSG_HANDLESHAPE, QString::number(m_nCurrentState));
}

//  切换为 手型 鼠标
void TitleWidget::on_HandleAction_trigger()
{
    m_nCurrentState = 1;

    QString btnName = Frame::sHandleShape;
    setHandleShapeBtn(btnName);

    m_pHandleAction->setChecked(true);
    m_pDefaultAction->setChecked(false);
}

//  切换为 默认鼠标
void TitleWidget::on_DefaultAction_trigger()
{
    m_nCurrentState = 0;

    QString btnName = Frame::sDefaultShape;
    setHandleShapeBtn(btnName);

    m_pHandleAction->setChecked(false);
    m_pDefaultAction->setChecked(true);
}

//  文件打开成功，　功能性　按钮才能点击
void TitleWidget::openFileOk()
{
    m_pThumbnailBtn->setDisabled(false);
    m_pSettingBtn->setDisabled(false);
    m_pHandleShapeBtn->setDisabled(false);
    m_pMagnifierBtn->setDisabled(false);
}

//  设置　手型按钮的图标
void TitleWidget::setHandleShapeBtn(const QString &btnName)
{
    QString normalPic = PF::getQrcPath(btnName, ImageModule::g_normal_state);
//    QString hoverPic = PF::getQrcPath(btnName, ImageModule::g_hover_state);
//    QString pressPic = PF::getQrcPath(btnName, ImageModule::g_press_state);
//    QString checkedPic = PF::getQrcPath(btnName, ImageModule::g_checked_state);

    m_pHandleShapeBtn->setIcon(QIcon(normalPic));
//    m_pHandleShapeBtn->setHoverPic(hoverPic);
//    m_pHandleShapeBtn->setPressPic(pressPic);
//    m_pHandleShapeBtn->setCheckedPic(checkedPic);
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
    m_layout->addWidget(m_pThumbnailBtn);

    m_pSettingBtn = createBtn(Frame::sSetting);
    connect(m_pSettingBtn, SIGNAL(clicked()), this, SLOT(on_settingBtn_clicked()));
    m_layout->addWidget(m_pSettingBtn);

    m_pHandleShapeBtn = createBtn(Frame::sDefaultShape);
    m_pHandleShapeBtn->setFixedSize(QSize(42, 36));
    m_pHandleShapeBtn->setIconSize(QSize(42, 36));
    connect(m_pHandleShapeBtn, SIGNAL(clicked()), this, SLOT(on_handleShapeBtn_clicked()));
    m_layout->addWidget(m_pHandleShapeBtn);

    m_pMagnifierBtn = createBtn(Frame::sMagnifier, true);
    connect(m_pMagnifierBtn, SIGNAL(clicked()), this, SLOT(on_magnifyingBtn_clicked()));
    m_layout->addWidget(m_pMagnifierBtn);
}

DIconButton *TitleWidget::createBtn(const QString &btnName, bool bCheckable)
{
    QString normalPic = PF::getQrcPath(btnName, ImageModule::g_normal_state);
//    QString hoverPic = PF::getQrcPath(btnName, ImageModule::g_hover_state);
//    QString pressPic = PF::getQrcPath(btnName, ImageModule::g_press_state);
//    QString disablePic = PF::getQrcPath(btnName, ImageModule::g_disable_state);

    DIconButton *btn = new  DIconButton(this);
    btn->setIcon(QIcon(normalPic));
    btn->setFixedSize(QSize(36, 36));
    btn->setIconSize(QSize(36, 36));
//    btn->setHoverPic(hoverPic);
//    btn->setPressPic(pressPic);

    btn->setToolTip(btnName);
    btn->setCheckable(bCheckable);
    if (bCheckable) {
        btn->setChecked(false);

//        QString checkedPic = PF::getQrcPath(btnName, ImageModule::g_checked_state);
//        btn->setCheckedPic(checkedPic);
    }

    btn->setDisabled(true);

    return btn;
}

//  创建两个按钮菜单， 对应 手型 和 默认鼠标
QAction *TitleWidget::createAction(const QString &actionName)
{
    QAction *_action = new QAction(actionName, this);
    _action->setCheckable(true);
    _action->setIcon(QIcon(QString(":/resources/image/normal/%1_small.svg").arg(actionName)));

    m_pHandleMenu->addAction(_action);
    return _action;
}

void TitleWidget::sendMsgToSubject(const int &msgType, const QString &msgCotent)
{
    sendMsg(msgType, msgCotent);
}

int TitleWidget::dealWithData(const int &msgType, const QString &msgContent)
{
    //  取消放大镜
    if (msgType == MSG_MAGNIFYING_CANCEL) {
        m_pMagnifierBtn->setChecked(false);
        sendMsgToSubject(MSG_MAGNIFYING, QString::number(0));
        return ConstantMsg::g_effective_res;
    }

    if (msgType == MSG_OPERATION_OPEN_FILE_OK) {
        openFileOk();
    } else if (msgType == MSG_OPERATION_FULLSCREEN) {
        m_pThumbnailBtn->setChecked(false);
    } else if (msgType == MSG_NOTIFY_KEY_MSG) {
        if (msgContent == "Esc") {      //  退出放大镜模式
            m_pMagnifierBtn->setChecked(false);
            sendMsgToSubject(MSG_MAGNIFYING, QString::number(0));
        }
    }
    return 0;
}

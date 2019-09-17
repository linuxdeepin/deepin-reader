#include "TitleWidget.h"
#include <QDebug>
#include <DImageButton>

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
    CustomImageButton *btn = reinterpret_cast<CustomImageButton *>(QObject::sender());
    int nHeight = btn->height();
    int nWidth = btn->width() / 2;

    QPoint point = btn->mapToGlobal(QPoint(0, 0));
    int nOldY = point.y();
    int nOldX = point.x();

    point.setX(nWidth + nOldX - 100);
    point.setY(nHeight + nOldY);
    qDebug() << "show setting font menu widget";
    m_pFontWidget->setGeometry(point.x(), point.y(), 200, 250);
    m_pFontWidget->show();
}

//  手型点击
void TitleWidget::on_handleShapeBtn_clicked()
{
    CustomImageButton *btn = reinterpret_cast<CustomImageButton *>(QObject::sender());
    int nHeight = btn->height();
    int nWidth = btn->width();

    QPoint point = btn->mapToGlobal(QPoint(0, 0));
    int nOldY = point.y();

    point.setY(nHeight + nOldY + 2);

    if (m_pHandleMenu == nullptr) {
        m_pHandleMenu = new DMenu;
        connect(m_pHandleMenu, SIGNAL(aboutToHide()), this, SLOT(slotHandleMenuHide()));

        m_pHandleAction = createAction(tr("hand"), SLOT(on_HandleAction_trigger(bool)));
        m_pHandleAction->setIcon(QIcon(":/resources/image/normal/hand_small.svg"));

        m_pDefaultAction = createAction(tr("choose"), SLOT(on_DefaultAction_trigger(bool)));
        m_pDefaultAction->setIcon(QIcon(":/resources/image/normal/choose_small.svg"));
        m_pDefaultAction->setChecked(true);
    }
    m_pHandleMenu->exec(point);
}

//  放大镜 功能
void TitleWidget::on_magnifyingBtn_clicked()
{
    bool bCheck = m_pMagnifierBtn->isChecked();
    sendMsgToSubject(MSG_MAGNIFYING, QString::number(bCheck));
}

//  切换为 手型 鼠标
void TitleWidget::on_HandleAction_trigger(bool)
{
    m_nCurrentState = 1;
    QString btnName = "hand";

    QString normalPic = PF::getQrcPath(btnName, ImageModule::g_normal_state);
    QString hoverPic = PF::getQrcPath(btnName, ImageModule::g_hover_state);
    QString pressPic = PF::getQrcPath(btnName, ImageModule::g_press_state);
    QString checkedPic = PF::getQrcPath(btnName, ImageModule::g_checked_state);

    m_pChooseBtn->setNormalPic(normalPic);
    m_pChooseBtn->setHoverPic(hoverPic);
    m_pChooseBtn->setPressPic(pressPic);
    m_pChooseBtn->setCheckedPic(checkedPic);

    m_pHandleAction->setChecked(true);
    m_pDefaultAction->setChecked(false);

    sendMsgToSubject(MSG_HANDLESHAPE, QString::number(m_nCurrentState));
}

//  切换为 默认鼠标
void TitleWidget::on_DefaultAction_trigger(bool)
{
    m_nCurrentState = 0;

    QString btnName = "choose";

    QString normalPic = PF::getQrcPath(btnName, ImageModule::g_normal_state);
    QString hoverPic = PF::getQrcPath(btnName, ImageModule::g_hover_state);
    QString pressPic = PF::getQrcPath(btnName, ImageModule::g_press_state);
    QString checkedPic = PF::getQrcPath(btnName, ImageModule::g_checked_state);

    m_pChooseBtn->setNormalPic(normalPic);
    m_pChooseBtn->setHoverPic(hoverPic);
    m_pChooseBtn->setPressPic(pressPic);
    m_pChooseBtn->setCheckedPic(checkedPic);

    m_pHandleAction->setChecked(false);
    m_pDefaultAction->setChecked(true);
    sendMsgToSubject(MSG_HANDLESHAPE, QString::number(m_nCurrentState));
}

//  文件打开成功，　功能性　按钮才能点击
void TitleWidget::openFileOk()
{
    m_pThumbnailBtn->setDisabled(false);
    m_pSettingBtn->setDisabled(false);
    m_pChooseBtn->setDisabled(false);
    m_pMagnifierBtn->setDisabled(false);
}

void TitleWidget::slotFontWidgetHide()
{
    m_pSettingBtn->setChecked(false);
}

void TitleWidget::slotHandleMenuHide()
{
    m_pChooseBtn->setChecked(false);
}

void TitleWidget::initBtns()
{
    m_pThumbnailBtn = createBtn(tr("thumbnail"), true);
    connect(m_pThumbnailBtn, SIGNAL(clicked()), this, SLOT(on_thumbnailBtn_clicked()));
    m_layout->addWidget(m_pThumbnailBtn);

    m_pSettingBtn = createBtn(tr("setting"));
    connect(m_pSettingBtn, SIGNAL(clicked()), this, SLOT(on_settingBtn_clicked()));
    m_layout->addWidget(m_pSettingBtn);

    m_pChooseBtn = createBtn(tr("choose"));
    connect(m_pChooseBtn, SIGNAL(clicked()), this, SLOT(on_handleShapeBtn_clicked()));
    m_layout->addWidget(m_pChooseBtn);

    m_pMagnifierBtn = createBtn(tr("magnifier"), true);
    connect(m_pMagnifierBtn, SIGNAL(clicked()), this, SLOT(on_magnifyingBtn_clicked()));
    m_layout->addWidget(m_pMagnifierBtn);
}

CustomImageButton *TitleWidget::createBtn(const QString &btnName, bool bCheckable)
{
    QString normalPic = PF::getQrcPath(btnName, ImageModule::g_normal_state);
    QString hoverPic = PF::getQrcPath(btnName, ImageModule::g_hover_state);
    QString pressPic = PF::getQrcPath(btnName, ImageModule::g_press_state);
    QString disablePic = PF::getQrcPath(btnName, ImageModule::g_disable_state);

    CustomImageButton *btn = new  CustomImageButton;
    btn->setNormalPic(normalPic);
    btn->setHoverPic(hoverPic);
    btn->setPressPic(pressPic);

    btn->setToolTip(btnName);
    btn->setCheckable(bCheckable);
    if (bCheckable) {
        btn->setChecked(false);

        QString checkedPic = PF::getQrcPath(btnName, ImageModule::g_checked_state);
        btn->setCheckedPic(checkedPic);
    }

    btn->setDisabled(true);

    return btn;
}

//  创建两个按钮菜单， 对应 手型 和 默认鼠标
QAction *TitleWidget::createAction(const QString &actionName, const char *member)
{
    QString iconName = actionName + "_small";
    QIcon icon = Utils::getActionIcon(iconName);

    QAction *_action = new QAction(actionName, this);
    _action->setIcon(icon);
    _action->setCheckable(true);
    connect(_action, SIGNAL(triggered(bool)), member);

    m_pHandleMenu->addAction(_action);
    return _action;
}

void TitleWidget::sendMsgToSubject(const int &msgType, const QString &msgCotent)
{
    sendMsg(msgType, msgCotent);
}

int TitleWidget::dealWithData(const int &msgType, const QString &)
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
    }
    return 0;
}

#include "TitleWidget.h"
#include <QDebug>
#include "header/CustomImageButton.h"


TitleWidget::TitleWidget(CustomWidget *parent) :
    CustomWidget("TitleWidget", parent)
{
    setFixedWidth(200);

    m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(6);
    setLayout(m_layout);

    initBtns();
}

void TitleWidget::initWidget()
{

}

//  缩略图 显示
void TitleWidget::on_thumbnailBtn_checkedChanged(bool bCheck)
{
    qDebug() << "   on_thumbnailBtn_checkedChanged";
    sendMsgToSubject(MSG_SLIDER_SHOW_STATE, QString::number(bCheck));
}

//  字体
void TitleWidget::on_fontBtn_clicked(bool)
{

}

//  手型点击
void TitleWidget::on_handleShapeBtn_clicked(bool)
{
    CustomImageButton *btn = reinterpret_cast<CustomImageButton *>(QObject::sender());
    int nHeight = btn->height();
    int nWidth = btn->width();

    QPoint point = btn->mapToGlobal(QPoint(0, 0));
    int nOldY = point.y();

    point.setY(nHeight + nOldY + 2);

    if (m_pHandleMenu == nullptr) {
        m_pHandleMenu = new DMenu(btn);

        createAction(tr("Handle"), SLOT(on_HandleAction_trigger(bool)));
        createAction(tr("Default"), SLOT(on_DefaultAction_trigger(bool)));
    }
    m_pHandleMenu->exec(point);
}

//  放大镜 功能
void TitleWidget::on_magnifyingBtn_checkedChanged(bool bCheck)
{
    sendMsgToSubject(MSG_MAGNIFYING, QString::number(bCheck));
}

//  切换为 手型 鼠标
void TitleWidget::on_HandleAction_trigger(bool)
{
    m_nCurrentState = 1;

    sendMsgToSubject(MSG_HANDLESHAPE, QString::number(m_nCurrentState));
}

//  切换为 默认鼠标
void TitleWidget::on_DefaultAction_trigger(bool)
{
    m_nCurrentState = 0;

    sendMsgToSubject(MSG_HANDLESHAPE, QString::number(m_nCurrentState));
}

//  初始化 标题栏 按钮
void TitleWidget::initBtns()
{
    createBtn("thumbnail", SLOT(on_thumbnailBtn_checkedChanged(bool)), true);
    createBtn("setting", SLOT(on_fontBtn_clicked(bool)), true);
    createBtn("choose", SLOT(on_handleShapeBtn_clicked(bool)), true);
    createBtn("magnifier", SLOT(on_magnifyingBtn_checkedChanged(bool)), true);
    m_layout->addStretch(1);
}

void TitleWidget::createBtn(const QString &btnName, const char *member, bool checkable, bool checked)
{
    QString normalPic = Utils::getQrcPath(btnName, g_normal_state);
    QString hoverPic = Utils::getQrcPath(btnName, g_hover_state);
    QString pressPic = Utils::getQrcPath(btnName, g_press_state);
    QString checkedPic = Utils::getQrcPath(btnName, g_checked_state);

//    qDebug() << "normalPic " << normalPic << " , hoverPic " << hoverPic
//             << ", pressPic " << pressPic << ", checkedPic " << checkedPic;

    CustomImageButton *btn = new CustomImageButton;
//    btn->setEnabled(false);
    btn->setStrNormalPic(normalPic);
    btn->setStrHoverPic(hoverPic);
    btn->setStrPressPic(pressPic);
    btn->setStrCheckPic(checkedPic);
//    DImageButton *btn = new DImageButton(normalPic, hoverPic, pressPic, checkedPic);
    btn->setToolTip(btnName);
//    btn->setCheckable(true);
//    btn->setChecked(false);

    if (checkable) {
        btn->setCheckable(true);
        btn->setChecked(checked);

//    connect(btn, SIGNAL(checkedChanged(bool)), member);
        connect(btn, SIGNAL(clicked(bool)), member);
    } else {
        connect(btn, SIGNAL(clicked()), member);
    }

    m_layout->addWidget(btn);
}

//  创建两个按钮菜单， 对应 手型 和 默认鼠标
void TitleWidget::createAction(const QString &iconName, const char *member)
{
    QIcon icon = Utils::getActionIcon(iconName);

    QAction *_action = new QAction(iconName, this);
    _action->setObjectName(iconName);
    _action->setIcon(icon);
    _action->setCheckable(true);
    connect(_action, SIGNAL(triggered(bool)), member);

    m_pHandleMenu->addAction(_action);
}
//    } else {
//        if (m_pDefaultAction == nullptr) {
//            m_pDefaultAction = new QAction(tr("Default"), this);
//            m_pDefaultAction->setIcon(icon);
//            m_pDefaultAction->setCheckable(true);

//            if (m_nCurrentState == 0) {
//                m_pDefaultAction->setChecked(true);
//            }
//            connect(m_pDefaultAction, SIGNAL(triggered()), member);
//        }

void TitleWidget::sendMsgToSubject(const int &msgType, const QString &msgContent)
{
    sendMsg(msgType, msgContent);
}

int TitleWidget::dealWithData(const int &msgType, const QString &)
{
    //  取消放大镜
    if (msgType == MSG_MAGNIFYING_CANCEL) {
        on_magnifyingBtn_checkedChanged(false);
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

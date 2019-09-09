#include "TitleWidget.h"
#include <QDebug>
#include <DImageButton>

TitleWidget::TitleWidget(CustomWidget *parent) :
    CustomWidget("TitleWidget", parent)
{
    setFixedWidth(200);

    m_layout = new QHBoxLayout();
    m_layout->setContentsMargins(0, 0, 0, 0);
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

    createBtn("thumbnail", 36, 36, SLOT(on_thumbnailBtn_clicked(bool)));
    createBtn("setting", 36, 36, SLOT(on_fontBtn_clicked(bool)));
    createBtn("choose", 42, 36, SLOT(on_handleShapeBtn_clicked(bool)));
    createBtn("magnifier", 36, 36, SLOT(on_magnifyingBtn_clicked(bool)));
    m_layout->addStretch(1);
}

//  缩略图 显示
void TitleWidget::on_thumbnailBtn_clicked(bool)
{
    sendMsgToSubject(MSG_SLIDER_SHOW_STATE);
}

//  字体
void TitleWidget::on_fontBtn_clicked(bool)
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
void TitleWidget::on_handleShapeBtn_clicked(bool)
{
    CustomImageButton *btn = reinterpret_cast<CustomImageButton *>(QObject::sender());
    int nHeight = btn->height();
    int nWidth = btn->width();

    QPoint point = btn->mapToGlobal(QPoint(0, 0));
    int nOldY = point.y();

    point.setY(nHeight + nOldY + 2);

    if (m_pHandleMenu == nullptr) {
        m_pHandleMenu = new DMenu;

        createAction(tr("Handle"), SLOT(on_HandleAction_trigger(bool)));
        createAction(tr("Default"), SLOT(on_DefaultAction_trigger(bool)));
    }
    m_pHandleMenu->exec(point);
}

//  放大镜 功能
void TitleWidget::on_magnifyingBtn_clicked(bool bCheck)
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

void TitleWidget::createBtn(const QString &btnName, const int &width, const int &height,  const char *member)
{
    QString normalPic = PF::getQrcPath(btnName, g_normal_state);
    QString hoverPic = PF::getQrcPath(btnName, g_hover_state);
    QString pressPic = PF::getQrcPath(btnName, g_press_state);
    QString checkedPic = PF::getQrcPath(btnName, g_checked_state);
    QString disablePic = PF::getQrcPath(btnName, g_disable_state);

    CustomImageButton *btn = new  CustomImageButton;
    btn->setNormalPic(normalPic);
    btn->setHoverPic(hoverPic);
    btn->setPressPic(pressPic);
    btn->setCheckedPic(checkedPic);

    btn->setToolTip(btnName);
    btn->setCheckable(true);
    btn->setChecked(false);

    connect(btn, &DImageButton::clicked, [btn] {
        qDebug() << "111111111111111        " << btn->isChecked();
        btn->setChecked(!btn->isChecked());
    });

    connect(btn, &DImageButton::checkedChanged, [btn](bool b) {
        qDebug() << "aaaa       " << btn->isChecked();
    });

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

void TitleWidget::sendMsgToSubject(const int &msgType, const QString &msgCotent)
{
    sendMsg(msgType, msgCotent);
}

int TitleWidget::dealWithData(const int &msgType, const QString &)
{
    //  取消放大镜
    if (msgType == MSG_MAGNIFYING_CANCEL) {
        on_magnifyingBtn_clicked(false);
        return ConstantMsg::g_effective_res;
    }
    return 0;
}

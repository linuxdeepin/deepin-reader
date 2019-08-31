#include "TitleWidget.h"
#include <DListWidget>
#include "utils.h"
#include <QDebug>


TitleWidget::TitleWidget(DWidget *parent) :
    DWidget(parent)
{
    setFixedWidth(200);

    m_layout = new QHBoxLayout();
    setLayout(m_layout);

    initBtns();

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->addObserver(this);
    }
}

TitleWidget::~TitleWidget()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
    }
}

//  缩略图 显示
void TitleWidget::on_thumbnailBtn_clicked(bool bCheck)
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_SLIDER_SHOW_STATE, QString::number(bCheck));
    }
}

//  字体
void TitleWidget::on_fontBtn_clicked()
{

}

//  手型点击
void TitleWidget::on_handleShapeBtn_clicked()
{
    DPushButton *btn = dynamic_cast<DPushButton *>(QObject::sender());
    int nHeight = btn->height();
    int nWidth = btn->width();

    QPoint point = btn->mapToGlobal(QPoint(0, 0));
    int nOldY = point.y();

    point.setY(nHeight + nOldY + 2);

    DMenu *pHandleMenu = new DMenu();
    pHandleMenu->setFixedWidth(nWidth);

    if (!m_bCurrentState) {
        createAction(tr("Handle"), SLOT(on_HandleAction_trigger()));
        pHandleMenu->addAction(m_pHandleAction);
    } else {
        createAction(tr("Default"), SLOT(on_DefaultAction_trigger()));
        pHandleMenu->addAction(m_pDefaultAction);
    }

    pHandleMenu->exec(point);
}

//  放大镜 功能
void TitleWidget::on_magnifyingBtn_clicked(bool bCheck)
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_MAGNIFYING, QString::number(bCheck));
    }
}

//  切换为 手型 鼠标
void TitleWidget::on_HandleAction_trigger()
{
    m_bCurrentState = true;
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_HANDLESHAPE, QString::number(m_bCurrentState));
    }
}

//  切换为 默认鼠标
void TitleWidget::on_DefaultAction_trigger()
{
    m_bCurrentState = false;
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(MSG_HANDLESHAPE, QString::number(m_bCurrentState));
    }
}

//  初始化 标题栏 按钮
void TitleWidget::initBtns()
{
    createBtn(tr("缩略图"), SLOT(on_thumbnailBtn_clicked(bool)), true);
    createBtn(tr("字体"), SLOT(on_fontBtn_clicked()));
    createBtn(tr("手型"), SLOT(on_handleShapeBtn_clicked()));
    createBtn(tr("放大镜"), SLOT(on_magnifyingBtn_clicked(bool)), true);
}

void TitleWidget::createBtn(const QString &iconName, const char *member, bool checkable, bool checked)
{
    DPushButton *btn = new DPushButton(iconName);

    if (checkable) {
        btn->setCheckable(true);
        btn->setChecked(checked);

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

    if (iconName == tr("Handle")) {
        if (m_pHandleAction == nullptr) {
            m_pHandleAction = new QAction(this);
            m_pHandleAction->setIcon(icon);

            connect(m_pHandleAction, SIGNAL(triggered()), member);
        }
    } else {
        if (m_pDefaultAction == nullptr) {
            m_pDefaultAction = new QAction(this);
            m_pDefaultAction->setIcon(icon);

            connect(m_pDefaultAction, SIGNAL(triggered()), member);
        }
    }
}

int TitleWidget::update(const int &, const QString &)
{
    return 0;
}

#include "TitleWidget.h"
#include "utils.h"
#include <QDebug>


TitleWidget::TitleWidget(CustomWidget *parent) :
    CustomWidget("TitleWidget", parent)
{
    setFixedWidth(200);

    m_layout = new QHBoxLayout();
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
void TitleWidget::on_fontBtn_clicked()
{

}

//  手型点击
void TitleWidget::on_handleShapeBtn_clicked()
{
    DImageButton *btn = reinterpret_cast<DImageButton *>(QObject::sender());
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
void TitleWidget::on_magnifyingBtn_checkedChanged(bool bCheck)
{
    sendMsgToSubject(MSG_MAGNIFYING, QString::number(bCheck));
}

//  切换为 手型 鼠标
void TitleWidget::on_HandleAction_trigger()
{
    m_bCurrentState = true;

    sendMsgToSubject(MSG_HANDLESHAPE, QString::number(m_bCurrentState));
}

//  切换为 默认鼠标
void TitleWidget::on_DefaultAction_trigger()
{
    m_bCurrentState = false;

    sendMsgToSubject(MSG_HANDLESHAPE, QString::number(m_bCurrentState));
}

//  初始化 标题栏 按钮
void TitleWidget::initBtns()
{
    createBtn(tr("缩略图"), tr(":/image/normal/thumbnail_normal_light.svg"), tr(""), tr(":/image/press/thumbnail_press_light.svg"), tr(""), SLOT(on_thumbnailBtn_checkedChanged(bool)), true);
    createBtn(tr("字体"), tr(""), tr(""), tr(""), tr(""), SLOT(on_fontBtn_clicked()));
    createBtn(tr("手型"), tr(""), tr(""), tr(""), tr(""), SLOT(on_handleShapeBtn_clicked()));
    createBtn(tr("放大镜"), tr(""), tr(""), tr(""), tr(""), SLOT(on_magnifyingBtn_checkedChanged(bool)), true);
}

void TitleWidget::createBtn(const QString &btnName, const QString &normalPic, const QString &hoverPic,
                            const QString &pressPic, const QString &checkedPic,
                            const char *member, bool checkable, bool checked)
{
    DImageButton *btn = new DImageButton(normalPic, hoverPic, pressPic, checkedPic);
    btn->setToolTip(btnName);
    btn->setCheckable(checkable);

    if (checkable) {
        btn->setChecked(checked);

        connect(btn, SIGNAL(checkedChanged(bool)), member);
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

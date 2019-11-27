#include "CustomWidget.h"
#include <DWidgetUtil>

CustomWidget::CustomWidget(const QString &name, DWidget *parent)
    : DWidget (parent)
{
    m_strObserverName = name;
    setWindowFlags(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::StrongFocus);
    setObjectName(name);
    setAutoFillBackground(true);
    setContextMenuPolicy(Qt::CustomContextMenu);//让widget支持右键菜单事件

    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->addObserver(this);
    }

    m_pNotifySubject = NotifySubject::getInstance();
    if (m_pNotifySubject) {
        m_pNotifySubject->addObserver(this);
    }
}

CustomWidget::~CustomWidget()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
    }
    if (m_pNotifySubject) {
        m_pNotifySubject->removeObserver(this);
    }
}

void CustomWidget::updateWidgetTheme()
{
//    auto plt = Dtk::Gui::DGuiApplicationHelper::instance()->applicationPalette();
//    plt.setColor(Dtk::Gui::DPalette::Background, plt.color(Dtk::Gui::DPalette::Base));
//    setPalette(plt);
}

//  操作 消息
void CustomWidget::sendMsg(const int &msgType, const QString &msgContent)
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(msgType, msgContent);
    }
}

//  通知消息
void CustomWidget::notifyMsg(const int &msgType, const QString &msgContent)
{
    if (m_pNotifySubject) {
        m_pNotifySubject->notifyMsg(msgType, msgContent);
    }
}

void CustomWidget::showScreenCenter()
{
    Dtk::Widget::moveToCenter(this);
    this->show();
}

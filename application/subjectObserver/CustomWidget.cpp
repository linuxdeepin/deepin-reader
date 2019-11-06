#include "CustomWidget.h"
#include <DWidgetUtil>

CustomWidget::CustomWidget(const QString &name, DWidget *parent)
    : DWidget (parent)
{
    m_strObserverName = name;
    setWindowFlags(Qt::FramelessWindowHint);
    setFocusPolicy(Qt::StrongFocus);
    setObjectName(name);

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

void CustomWidget::sendMsg(const int &msgType, const QString &msgContent)
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(this, msgType, msgContent);
    }
}

void CustomWidget::showScreenCenter()
{
    Dtk::Widget::moveToCenter(this);
    this->show();
}

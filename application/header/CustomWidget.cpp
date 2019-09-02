#include "CustomWidget.h"

CustomWidget::CustomWidget(DWidget *parent)
    : DWidget (parent)
{
    m_pMsgSubject = MsgSubject::getInstance();
    if (m_pMsgSubject) {
        m_pMsgSubject->addObserver(this);
    }
}

CustomWidget::~CustomWidget()
{
    if (m_pMsgSubject) {
        m_pMsgSubject->removeObserver(this);
    }
}

void CustomWidget::sendMsg(const int &msgType, const QString &msgContent)
{
    if (m_pMsgSubject) {
        m_pMsgSubject->sendMsg(msgType, msgContent);
    }
}

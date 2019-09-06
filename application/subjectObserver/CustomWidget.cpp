#include "CustomWidget.h"

CustomWidget::CustomWidget(const QString &name, DWidget *parent)
    : DWidget (parent)
{
    setObserverName(name);

    //  主题变化
    connect(ViewerThemeManager::instance(), SIGNAL(viewerThemeChanged(AppTheme)),
            this, SLOT(slotViewerThemeChanged(AppTheme)));

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
        m_pMsgSubject->sendMsg(this, msgType, msgContent);
    }
}

void CustomWidget::setObserverName(const QString &name)
{
    m_strObserverName = name;
}

void CustomWidget::slotViewerThemeChanged(AppTheme theme)
{
    qDebug() << m_strObserverName << "      slotViewerThemeChanged     "   << theme;
}

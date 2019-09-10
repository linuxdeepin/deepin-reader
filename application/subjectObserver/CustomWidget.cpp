#include "CustomWidget.h"
#include <DWidgetUtil>

CustomWidget::CustomWidget(const QString &name, DWidget *parent)
    : DWidget (parent)
{
    setObserverName(name);

    initThemeChanged();

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

void CustomWidget::showScreenCenter()
{
    Dtk::Widget::moveToCenter(this);
    this->show();
}

//  主题变化
void CustomWidget::initThemeChanged()
{
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged,
    [](DGuiApplicationHelper::ColorType type) {
        qDebug() << "       " << type;
    });
}

void CustomWidget::setObserverName(const QString &name)
{
    m_strObserverName = name;
}

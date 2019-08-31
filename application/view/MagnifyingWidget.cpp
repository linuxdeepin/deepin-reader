#include "MagnifyingWidget.h"
#include <QPainter>

MagnifyingWidget::MagnifyingWidget(DWidget *parent) :
    DWidget (parent)
{
    this->setFixedSize(QSize(50, 50));
    this->setVisible(false);    //  默认隐藏

    m_pMsgSubject = MsgSubject::getInstance();
    if(m_pMsgSubject)
    {
        m_pMsgSubject->addObserver(this);
    }
}

MagnifyingWidget::~MagnifyingWidget()
{
    if(m_pMsgSubject)
    {
        m_pMsgSubject->removeObserver(this);
    }
}

void MagnifyingWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.save();

    QRect rect = this->geometry();
    painter.setBrush(QBrush(QColor(255,0,0)));
    painter.drawEllipse(0, 0, rect.height(), rect.width());

    painter.restore();
}

int MagnifyingWidget::update(const int&, const QString &)
{
    return 0;
}

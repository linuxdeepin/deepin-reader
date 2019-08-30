#include "MagnifyingWidget.h"
#include <QPainter>

MagnifyingWidget::MagnifyingWidget(DWidget *parent) :
    DWidget (parent)
{
    this->setFixedSize(QSize(50, 50));
    this->setVisible(false);    //  默认隐藏

    m_pThemeSubject = ThemeSubject::getInstace();
    if(m_pThemeSubject)
    {
        m_pThemeSubject->addObserver(this);
    }
}

MagnifyingWidget::~MagnifyingWidget()
{
    if(m_pThemeSubject)
    {
        m_pThemeSubject->removeObserver(this);
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

int MagnifyingWidget::update(const QString &)
{
    return 0;
}

#include "MagnifyingWidget.h"
#include <QPainter>

MagnifyingWidget::MagnifyingWidget(CustomWidget *parent)
    : CustomWidget(parent)
{
    this->setFixedSize(QSize(50, 50));
    this->setVisible(false); //  默认隐藏
}

void MagnifyingWidget::paintEvent(QPaintEvent *event)
{
    DWidget::paintEvent(event);

    QPainter painter(this);
    painter.save();

    QRect rect = this->geometry();
    painter.setBrush(QBrush(QColor(255, 0, 0)));
    painter.drawEllipse(0, 0, rect.height(), rect.width());

    painter.restore();
}

int MagnifyingWidget::update(const int &, const QString &)
{
    return 0;
}

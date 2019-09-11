#include "CustomLabel.h"

CustomLabel::CustomLabel(QWidget *parent) :
    QLabel(parent)
{

}

void CustomLabel::paintEvent(QPaintEvent *e)
{
    QRectF rectangle(0.0, 0.0, (this->width() * 1.0), (this->height() * 1.0));

    QPainter painter(this);
    painter.setOpacity(1);
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawRoundedRect(rectangle, 15, 15);

    return QLabel::paintEvent(e);
}

#include "MyLabel.h"

MyLabel::MyLabel(QWidget *parent) :
    QLabel(parent)
{

}

MyLabel::~MyLabel()
{

}

void MyLabel::paintEvent(QPaintEvent *e)
{
    QRectF rectangle(0.0, 0.0, this->width() - 20, this->height());

    QPainter painter(this);
    painter.setRenderHint( QPainter::Antialiasing, true );
    painter.setBrush(QBrush(QColor(255, 255, 255)));
    painter.drawRoundedRect(rectangle, 10, 10);

    return QLabel::paintEvent(e);
}

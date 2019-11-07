#include "CustomLabel.h"

CustomLabel::CustomLabel(QWidget *parent) :
    DLabel(parent)
{

}

/**
 * @brief CustomLabel::paintEvent
 * 自绘承载缩略图标签
 * @param e
 */
void CustomLabel::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    DLabel::paintEvent(e);
    QRectF rectangle(2, 2, (this->width() - 4), (this->height() - 4));

    QPainter painter(this);
    painter.setOpacity(1);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);
//    painter.setPen(QPen(QColor(QString("#0081FF")), 3));
    painter.drawRoundedRect(rectangle, 8, 8);

}

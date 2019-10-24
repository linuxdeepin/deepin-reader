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
    QRectF rectangle(1, 1, (this->width() - 2), (this->height() - 2));

    QPainter painter(this);
    painter.setOpacity(1);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rectangle, 10, 10);

    DLabel::paintEvent(e);
}

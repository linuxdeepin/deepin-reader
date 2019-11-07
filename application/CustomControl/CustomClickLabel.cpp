#include "CustomClickLabel.h"

CustomClickLabel::CustomClickLabel(const QString &text, DWidget *parent, Qt::WindowFlags f)
    : DLabel (text, parent, f)
{

}

/**
* @brief CustomClickLabel::mousePressEvent
* 自定义label点击事件
* @param event
*/

void CustomClickLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }

    DLabel::mousePressEvent(event);
}

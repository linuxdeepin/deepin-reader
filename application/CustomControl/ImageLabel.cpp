#include "ImageLabel.h"
#include <QPainter>

ImageLabel::ImageLabel(DWidget *parent)
    : DLabel (parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
}

void ImageLabel::setSelect(const bool &select)
{
    m_bSelect = select;
    update();
}

void ImageLabel::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    DLabel::paintEvent(e);
    qreal local = 0;
    qreal width = 0;
    qreal heigh = 0;

    if (m_bSelect) {
        local = 3;
        width = this->width() - 6;
        heigh = this->height() - 6;
    } else {
        local = 0;
        width = this->width() - 0;
        heigh = this->height() - 0;
    }
    QRectF rectangle(local, local, width, heigh);

    QPainter painter(this);
    painter.setOpacity(1.0);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(rectangle, 8, 8);

    DLabel::paintEvent(e);
}

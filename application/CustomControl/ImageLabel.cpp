#include "ImageLabel.h"
#include <QPainter>
#include <QPalette>

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
    int local = 0;
    int width = this->width();
    int heigh = this->height();
    int penwidth=0;
    QPalette p(this->palette());
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    if (m_bSelect) {
        local = 1;
        width -= 2;
        heigh -= 2;
        penwidth=3;
        painter.setPen(QPen(/*QColor(QString("#0081FF"))*/p.highlight().color(), penwidth, Qt::SolidLine));
    } else {
        local = 3;
        width -= 6;
        heigh -= 6;
        penwidth=1;
        painter.setPen(QPen(QColor::fromRgbF(0, 0, 0, 0.08)/*p.shadow().color()*/, penwidth, Qt::SolidLine));
    }

    QRectF rectangle(local, local, width, heigh);
    painter.drawRoundedRect(rectangle, m_nRadius, m_nRadius);
    DLabel::paintEvent(e);
}

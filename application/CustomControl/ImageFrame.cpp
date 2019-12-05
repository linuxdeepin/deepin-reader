#include "ImageFrame.h"
#include <QPainter>

ImageFrame::ImageFrame(QWidget *parent)
    : DFrame(parent)
{
}

void ImageFrame::setBackgroundPix(QPixmap pixmap)
{
    m_background = pixmap;
    update();
}

void ImageFrame::paintEvent(QPaintEvent *e)
{
    DFrame::paintEvent(e);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    //填充图片
    QPainterPath path;
    QRectF pixrectangle(1, 1, this->width() - 2, this->height() - 2);
    path.addRoundedRect(pixrectangle, 8, 8);
    painter.setClipPath(path);
    QRect bprectangle(1, 1, this->width() - 2, this->height() - 2);
    painter.drawPixmap(bprectangle, m_background);
}

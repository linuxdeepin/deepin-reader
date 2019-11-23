#include "ColorMenu.h"
#include <QPainter>
#include <QPen>
#include <QMouseEvent>
#include <DStyle>

static const int EXTRA = 2; //2px extra space to avoid line cutted off

ColorMenu::ColorMenu(const QColor &color, QWidget *parent) :
    DWidget(parent), m_color(color)
{

}

void ColorMenu::setSelect(const bool select)
{
    if(m_bSelected == select){
        return;
    }

    m_bSelected = select;
    update();
}

void ColorMenu::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton) {
        if (m_bSelected) return;
        Q_EMIT clicked();
    }
}

void ColorMenu::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    DWidget::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    int borderWidth = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderWidth), nullptr, this);
    int borderSpacing = style()->pixelMetric(static_cast<QStyle::PixelMetric>(DStyle::PM_FocusBorderSpacing), nullptr, this);
    int totalSpace = borderWidth + borderSpacing;
    QRect squareRect = rect();
    int delta = (squareRect.width() - squareRect.height())/2;

    if (delta != 0){
        squareRect = (delta > 0) ? squareRect.adjusted(delta + EXTRA, EXTRA, -delta - EXTRA, -EXTRA)
                                 : squareRect.adjusted(EXTRA, -delta + EXTRA , -EXTRA, delta - EXTRA); //adjust it to square
    }

    if (m_bSelected) {
        //draw select circle
        QPen pen;
        pen.setBrush(palette().highlight());
        pen.setWidth(borderWidth);  //pen width
        painter.setPen(pen);
        painter.drawEllipse(squareRect);
    }

    QPainterPath path;
    QRect r = squareRect.adjusted(totalSpace, totalSpace, -totalSpace, -totalSpace);
    path.addEllipse(r);
    painter.setClipPath(path);
    painter.drawPath(path);
    painter.fillPath(path, QBrush(m_color));
}

#include "bookmarkbutton.h"

#include<QColor>
#include<QBrush>
#include<QPen>
#include<QPoint>
#include<QPainter>
#include <DApplicationHelper>

BookMarkButton::BookMarkButton(DWidget *parent)
    : DPushButton(parent),
      ishovered(false),
      ispressed(false),
      isclicked(false)
{
    setFixedSize(QSize(30, 30));
    setMouseTracking(true);
}


bool BookMarkButton::clickState()
{
    return isclicked;
}
void BookMarkButton::setClickState(bool state)
{
    isclicked = state;
    update();
}

void BookMarkButton::paintEvent(QPaintEvent *e)

{
    QPainter painter;
    painter.begin(this);
    QString ssPath = ":/resources/image/";
    DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
    if (themeType == DGuiApplicationHelper::LightType)
        ssPath += "light";
    else if (themeType == DGuiApplicationHelper::DarkType)
        ssPath += "dark";
    if (isclicked) {
        ssPath += "/checked/bookmarkbig_checked_light.svg";
    } else {
        if (ispressed) {
            ssPath += "/press/bookmarkbig_press_light.svg";
        } else {
            if (ishovered) {
                ssPath += "/hover/bookmarkbig_hover_light.svg";
            }
        }
    }
    QPixmap pixmap(ssPath);
    const qreal ratio = qApp->devicePixelRatio();
    pixmap.setDevicePixelRatio(ratio);
//    pixmap = pixmap.scaled(this->width(), this->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(0, 0, this->width(), this->height(), pixmap);
    painter.end();
}

void BookMarkButton::enterEvent(QEvent *e)

{
    ishovered = true;
    repaint();
}

void BookMarkButton::leaveEvent(QEvent *e)

{
    ishovered = false;
    repaint();
}


void BookMarkButton::mousePressEvent(QMouseEvent *e)
{
    ispressed = true;
    repaint();
}

void BookMarkButton::mouseReleaseEvent(QMouseEvent *e)
{
    ispressed = false;
    isclicked = !isclicked;
    emit signal_bookMarkStateChange(isclicked);
    repaint();
}

//void BookMarkButton::mouseEvent(QMouseEvent *e)

//{
//    float  w = this->width();
//    float  h = this->height();
//    int  x = e->x();
//    int  y = e->y();
//    float k = h / w;
//    if ( y > -k * x + h / 2 &&

//            y >= k * x - h / 2 &&

//            y <= k * x + h / 2 &&

//            y <= -k * x + 3 * h / 2) {
//        ishovered = true;
//    } else {
//        ishovered = false;
//    }
//    repaint();
//}

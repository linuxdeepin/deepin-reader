#include "bookmarkbutton.h"

#include<QColor>
#include<QBrush>
#include<QPen>
#include<QPoint>
#include<QPainter>

BookMarkButton::BookMarkButton(QWidget *parent)
    : QPushButton(parent), ishovered(false), ispressed(false), isclicked(false)
{
    setFixedSize(QSize(39, 39));
    setMouseTracking(true);
}


bool BookMarkButton::clickState()
{
    return isclicked;
}
void BookMarkButton::setClickState(bool state)
{
    isclicked = state;
}

void BookMarkButton::paintEvent(QPaintEvent *e)

{
    QPainter painter;
    painter.begin(this);
    QString ssPath = ":/resources/image/normal/bookmark.svg";
    if (isclicked) {
        ssPath = ":/resources/image/checked/bookmark.svg";
    } else {
        if (ispressed) {
            ssPath = ":/resources/image/press/bookmark.svg";
        } else {
            if (ishovered) {
                ssPath = ":/resources/image/hover/bookmark.svg";
            }
        }
    }
    QPixmap pixmap(ssPath);
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

void BookMarkButton::mouseEvent(QMouseEvent *e)

{
    float  w = this->width();
    float  h = this->height();
    int  x = e->x();
    int  y = e->y();
    float k = h / w;
    if ( y > -k * x + h / 2 &&

            y >= k * x - h / 2 &&

            y <= k * x + h / 2 &&

            y <= -k * x + 3 * h / 2) {
        ishovered = true;
    } else {
        ishovered = false;
    }
    repaint();
}

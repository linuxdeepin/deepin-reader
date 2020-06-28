#include "bookmarkbutton.h"
#include "utils/utils.h"
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
    update();
}

void BookMarkButton::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QString iconPath;
    QPixmap pixmap;
    if (isclicked) {
        pixmap = QIcon::fromTheme("dr_bookmark_checked").pixmap(QSize(39, 39));
    } else {
        if (ispressed) {
            pixmap = QIcon::fromTheme("dr_bookmark_pressed").pixmap(QSize(39, 39));
        } else {
            if (ishovered) {
                pixmap = QIcon::fromTheme("dr_bookmark_hover").pixmap(QSize(39, 39));
            }
        }
    }

    painter.drawPixmap(0, 0, this->width(), this->height(), pixmap);
}

void BookMarkButton::enterEvent(QEvent *)
{
    ishovered = true;
    repaint();
}

void BookMarkButton::leaveEvent(QEvent *)
{
    ishovered = false;
    repaint();
}


void BookMarkButton::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        ispressed = true;
        repaint();
    }
}

void BookMarkButton::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        ispressed = false;

        emit sigClicked(!isclicked);

        repaint();
    }
}

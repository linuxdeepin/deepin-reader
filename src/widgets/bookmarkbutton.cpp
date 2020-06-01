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
    setFixedSize(QSize(36, 36));
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
    QPainter painter(this);
    QString iconPath;
    if (isclicked) {
        iconPath += ":/icons/deepin/builtin/bookmark.svg";
    } else {
        if (ispressed) {
            iconPath += ":/icons/deepin/builtin/bookmark_pressed.svg";
        } else {
            if (ishovered) {
                iconPath += ":/icons/deepin/builtin/bookmark_hover.svg";
            }
        }
    }

    QPixmap pixmap(iconPath);
    painter.drawPixmap(0, 0, this->width(), this->height(), pixmap);
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

    emit sigClicked(!isclicked);

    repaint();
}

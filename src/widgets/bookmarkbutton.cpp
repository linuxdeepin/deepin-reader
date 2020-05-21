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
    QPainter painter;
    painter.begin(this);

    QString iconPath;
    if (isclicked) {
        iconPath += ":/custom/bookmark.svg";
    } else {
        if (ispressed) {
            iconPath += ":/custom/bookmark_pressed.svg";
        } else {
            if (ishovered) {
                iconPath += ":/custom/bookmark_hover.svg";
            }
        }
    }
    QPixmap pixmap(Utils::renderSVG(iconPath, QSize(36, 36)));
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

    emit sigClicked(!isclicked);

    repaint();
}

#include "CustomImageButton.h"
#include <QDebug>
#include <QFile>

CustomImageButton::CustomImageButton(DWidget *parent)
    : DImageButton (parent)
{
}

/*
void CustomImageButton::enterEvent(QEvent *event)
{
    QString sHoverPic = this->getHoverPic();
    if ( QFile::exists(sHoverPic)) {
        qDebug() << "111        " << sHoverPic << "         123";
    } else {
        qDebug() << "111    111111111111111         123";
    }
    DImageButton::enterEvent(event);
}

void CustomImageButton::leaveEvent(QEvent *event)
{
    qDebug() << "222";
    DImageButton::leaveEvent(event);
}

void CustomImageButton::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "333";
    DImageButton::mousePressEvent(event);
}

void CustomImageButton::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "444";
    DImageButton::mouseReleaseEvent(event);
}

void CustomImageButton::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "555";
    DImageButton::mouseMoveEvent(event);
}

*/

void CustomImageButton::paintEvent(QPaintEvent *event)
{
    State curState = getState();
    if (curState == Normal) {
//        drawNorPic();
    } else if (curState == Hover) {
        drawHoverPic();
    } else if (curState == Press) {
        drawPressPic();
    } else if (curState == Checked) {

    } else if (curState == Disabled) {

    }

    DImageButton::paintEvent(event);
}

void CustomImageButton::drawNorPic()
{
    QPainter painter(this);

    painter.setPen(QPen(Qt::red, 1));
    painter.setBrush(Qt::NoBrush);
    painter.drawRoundedRect(0, 0, this->width(), this->height(), 8, 8);
}

void CustomImageButton::drawHoverPic()
{
    QString sHoverPic = this->getHoverPic();
    if ( QFile::exists(sHoverPic)) {
        qDebug() << "111        " << sHoverPic << "         123";
    } else {
        QString strNoPic = this->getNormalPic();
        this->setHoverPic(strNoPic);
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    painter.setPen(QPen(QColor(242, 242, 242)));

    painter.setBrush(QColor(242, 242, 242));
    painter.drawRoundedRect(0, 0, this->width(), this->height(), 8, 8);
}

void CustomImageButton::drawPressPic()
{
    QString sPressPic = this->getPressPic();
    if ( QFile::exists(sPressPic)) {
        qDebug() << "111        " << sPressPic << "         123";
    } else {
        QString strNoPic = this->getNormalPic();
        this->setPressPic(strNoPic);
    }
    QPainter painter(this);
    painter.setRenderHints(QPainter::SmoothPixmapTransform);

    painter.setPen(QPen(QColor(230, 230, 230)));

    painter.setBrush(QColor(230, 230, 230));
    painter.drawRoundedRect(0, 0, this->width(), this->height(), 8, 8);
}

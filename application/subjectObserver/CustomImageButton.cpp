#include "CustomImageButton.h"
#include <QFile>

#include <DGuiApplicationHelper>
DGUI_USE_NAMESPACE

#include <QDebug>

CustomImageButton::CustomImageButton(DWidget *parent)
    : DImageButton (parent)
{
    setFixedHeight(36);
    initThemeChanged();
}

void CustomImageButton::paintEvent(QPaintEvent *event)
{
    State curState = getState();
    if (curState == Normal) {
    } else if (curState == Hover) {
        drawHoverPic();
    } else if (curState == Press) {
        drawPressPic();
    } else if (curState == Checked) {
        drawCheckedPic();
    } else if (curState == Disabled) {

    }

    DImageButton::paintEvent(event);
}

void CustomImageButton::initThemeChanged()
{
    connect(DGuiApplicationHelper::instance(), &DGuiApplicationHelper::paletteTypeChanged,
    [](DGuiApplicationHelper::ColorType type) {
        qDebug() << "       " << type;
    });
}

void CustomImageButton::drawHoverPic()
{
    QString sHoverPic = this->getHoverPic();
    if ( QFile::exists(sHoverPic)) {
    } else {
        QString strNoPic = this->getNormalPic();
        this->setHoverPic(strNoPic);
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setPen(QPen(QColor(242, 242, 242)));

    painter.setBrush(QColor(242, 242, 242));
    int nnWidth = this->width();
    int nnHeight = this->height();

    painter.drawRoundedRect(0, 0, nnWidth, nnHeight, 8, 8);
}

void CustomImageButton::drawPressPic()
{
    QString sPressPic = this->getPressPic();
    if ( QFile::exists(sPressPic)) {
    } else {
        QString strNoPic = this->getNormalPic();
        this->setPressPic(strNoPic);

        QPainter painter(this);
        painter.setRenderHints(QPainter::Antialiasing);

        painter.setPen(QPen(QColor(230, 230, 230)));

        painter.setBrush(QColor(230, 230, 230));
        painter.drawRoundedRect(0, 0, this->width(), this->height(), 8, 8);
    }
}

void CustomImageButton::drawCheckedPic()
{
    QString sCheckPic = this->getCheckedPic();
    if ( QFile::exists(sCheckPic)) {
    } else {
        QString strNoPic = this->getNormalPic();
        this->setCheckedPic(strNoPic);
    }
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    painter.setPen(QPen(QColor("#0081FF")));

    painter.setBrush(QColor("#0081FF"));
    painter.drawRoundedRect(0, 0, this->width(), this->height(), 8, 8);
}


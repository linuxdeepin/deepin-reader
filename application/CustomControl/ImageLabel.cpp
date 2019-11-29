#include "ImageLabel.h"
#include "utils/utils.h"
#include <QPainter>
#include <QPalette>
#include <DGuiApplicationHelper>
#include <QDebug>

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
        penwidth=5;
        painter.setPen(QPen(DGuiApplicationHelper::instance()->applicationPalette().highlight().color(), penwidth, Qt::SolidLine));
    } else {
        local = 2;
        width -= 4;
        heigh -= 4;
        penwidth=1;
        painter.setPen(QPen(DGuiApplicationHelper::instance()->applicationPalette().frameShadowBorder().color(), penwidth, Qt::SolidLine));
    }

    if(m_bSetBp){
        //填充图片
        QPainterPath path;
        QRectF pixrectangle(local, local, width, heigh);
        path.addRoundedRect(pixrectangle, m_nRadius, m_nRadius);
        painter.setClipPath(path);
        QRect bprectangle(local, local, width, heigh);
        painter.drawPixmap(bprectangle, m_background);
        QRectF rectangle(local, local, width, heigh);
        painter.drawRoundedRect(rectangle, m_nRadius, m_nRadius);
    }

    DLabel::paintEvent(e);
    if(m_bshowbookmark)
    {
        QString ssPath = ":/resources/image/";
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType)
            ssPath += "light";
        else if (themeType == DGuiApplicationHelper::DarkType)
            ssPath += "dark";

        ssPath += "/checked/bookmarkbig_checked_light.svg";
        QPixmap pixmap(Utils::renderSVG(ssPath,/*QSize(16,16)*/QSize(36,36)));
        QPainter painter1(this);
        painter1.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        painter1.drawPixmap(this->width()-36-6,0, 36,36, pixmap);
    }
}

/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "ImageLabel.h"

#include <DGuiApplicationHelper>
#include <QDebug>
#include <QPainter>
#include <QPalette>

#include "utils/utils.h"

ImageLabel::ImageLabel(DWidget *parent)
    : DLabel(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setScaledContents(true);
}

void ImageLabel::setSelect(const bool &select)
{
    m_bSelect = select;
    if (m_bSelect) {
        if (m_nRadius == 8) {
            m_nHighLightLineWidth = 4;
        } else if (m_nRadius == 10) {
            m_nHighLightLineWidth = 6;
        }
    }
    update();
}

void ImageLabel::setBackgroundPix(QPixmap &pixmap)
{
    m_background = pixmap;
    m_bSetBp = true;
    m_thumbPix = pixmap;
    setHasThumbnail(true);

    rotateImage();
}

void ImageLabel::rotateImage()
{
    m_thumbPix = m_background;
    QMatrix leftmatrix;
    leftmatrix.rotate(m_nRotate);
    m_thumbPix = m_background.transformed(leftmatrix, Qt::SmoothTransformation);
    m_thumbPix.setDevicePixelRatio(devicePixelRatioF());

    int w = this->width();
    int h = this->height();

    if (m_nRotate % 180 == 0) {
        if (w > h) {
            w = this->height();
            h = this->width();
        }
    } else {
        if (w < h) {
            w = this->height();
            h = this->width();
        }
    }

    setFixedSize(w, h);

    update();
}

void ImageLabel::setRotateAngle(const int &angle)
{
    if (angle == 1) {
        m_nRotate = 90;
    } else if (angle == 2) {
        m_nRotate = 180;
    } else if (angle == 3) {
        m_nRotate = 270;
    } else {
        m_nRotate = 0;
    }
}

/**
 * @brief ImageLabel::scaleImage
 * 缩略图列表自适应视窗大小
 * @param scale  缩放因子 大于0的数
 */
void ImageLabel::scaleImage(const double &scale)
{
    double width = 1.0;
    double height = 1.0;

    int w = m_size.width();
    int h = m_size.height();

    if (m_nRotate % 180) {
        height = static_cast<double>(w) * scale;
        width = static_cast<double>(h) * scale;
    } else {
        width = static_cast<double>(w) * scale;
        height = static_cast<double>(h) * scale;
    }

    setFixedSize(static_cast<int>(width), static_cast<int>(height));

    update();
}

/**
 * @brief ImageLabel::scaleImage
 * 缩略图大小
 * @param twidth     缩略图宽
 * @param theight    缩略图高
 */
void ImageLabel::scaleImage(const int &twidth, const int &theight)
{
    setFixedSize(static_cast<int>(twidth), static_cast<int>(twidth));
    update();
}

void ImageLabel::paintEvent(QPaintEvent *e)
{
    DLabel::paintEvent(e);

    int local = 0;
    int width = this->width();
    int heigh = this->height();
    int penwidth = 0;
    QPalette p(this->palette());
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    penwidth = m_nHighLightLineWidth;

    if (m_bSelect) {
        local = 1;
        width -= 2;
        heigh -= 2;
        //        penwidth = 8;
        painter.setPen(
            QPen(DGuiApplicationHelper::instance()->applicationPalette().highlight().color(),
                 penwidth, Qt::SolidLine));
    } else {
        local = 1;
        width -= 2;
        heigh -= 2;
        penwidth = 1;
        painter.setPen(QPen(
                           DGuiApplicationHelper::instance()->applicationPalette().frameShadowBorder().color(),
                           penwidth, Qt::SolidLine));
    }

    if (m_bSetBp) {
        //填充图片
        QPainterPath path;
        QRectF pixrectangle(local, local, width, heigh);
        path.addRoundedRect(pixrectangle, m_nRadius, m_nRadius);
        painter.setClipPath(path);
        QRect bprectangle(local, local, width, heigh);
        painter.drawPixmap(bprectangle, /*m_background*/ m_thumbPix);
        QRectF rectangle(local, local, width, heigh);
        painter.drawRoundedRect(rectangle, m_nRadius, m_nRadius);
    }

    if (m_bshowbookmark) {
        QString ssPath = ":/resources/image/";
        DGuiApplicationHelper::ColorType themeType = DGuiApplicationHelper::instance()->themeType();
        if (themeType == DGuiApplicationHelper::LightType)
            ssPath += "light";
        else if (themeType == DGuiApplicationHelper::DarkType)
            ssPath += "dark";

        ssPath += "/checked/bookmarkbig_checked_light.svg";
        QPixmap pixmap(Utils::renderSVG(ssPath, /*QSize(16,16)*/ QSize(36, 36)));
        QPainter painter1(this);
        painter1.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
        painter1.drawPixmap(this->width() - 36 - 6, 0, 36, 36, pixmap);
    }

    //    m_bRotate = false;
}

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
#include "RotateImageLabel.h"

#include <DGuiApplicationHelper>
#include <QDebug>
#include <QPainter>
#include <QPalette>

#include "utils/utils.h"

RotateImageLabel::RotateImageLabel(DWidget *parent)
    : DLabel(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setScaledContents(true);
}

void RotateImageLabel::setSelect(const bool &select)
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

void RotateImageLabel::setBackgroundPix(QPixmap &pixmap)
{
    m_background = pixmap;

    m_bSetBp = true;

    m_thumbPix = pixmap;

    setHasThumbnail(true);

    rotateImage();
}

void RotateImageLabel::rotateImage()
{
    m_thumbPix = m_background;

    QMatrix leftmatrix;

    leftmatrix.rotate(m_nRotate);

    m_thumbPix = m_background.transformed(leftmatrix, Qt::SmoothTransformation);

    m_thumbPix.setDevicePixelRatio(devicePixelRatioF());

    setFixedSize(m_thumbPix.size().width() * m_scale, m_thumbPix.height()*m_scale);

    update();
}

void RotateImageLabel::setRotateAngle(const int &angle)
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

void RotateImageLabel::scaleImage(const double &scale)
{
    m_scale = scale;

    setFixedSize(m_thumbPix.size().width() * m_scale, m_thumbPix.height()*m_scale);

    update();
}

void RotateImageLabel::paintEvent(QPaintEvent *e)
{
    DLabel::paintEvent(e);

    int local = 0;
    int width = this->width();
    int heigh = this->height();
    int penwidth = 0;

    QPainter painter(this);

    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    penwidth = m_nHighLightLineWidth;

    if (m_bSelect) {
        local = 1;
        width -= 2;
        heigh -= 2;
        painter.setPen(
            QPen(DGuiApplicationHelper::instance()->applicationPalette().highlight().color(),
                 penwidth, Qt::SolidLine));
    } else {
        local = 2;
        width -= 4;
        heigh -= 4;
        penwidth = 1;
        painter.setPen(QPen(
                           DGuiApplicationHelper::instance()->applicationPalette().frameShadowBorder().color(),
                           penwidth, Qt::SolidLine));
    }

    if (m_bSetBp) {
        //填充图片
        QPainterPath path;
        QRectF pixrectangle(local, local, width, heigh);
        path.addRoundedRect(pixrectangle, 8, 8);
        painter.setClipPath(path);
        QRect bprectangle(local, local, width, heigh);
        painter.drawPixmap(bprectangle, m_thumbPix);
        QRectF rectangle(local, local, width, heigh);
        painter.drawRoundedRect(rectangle, 8, 8);
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
}

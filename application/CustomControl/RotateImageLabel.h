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
#ifndef RotateImageLabel_H
#define RotateImageLabel_H

#include <DLabel>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class RotateImageLabel : public DLabel
{
    Q_OBJECT
    Q_DISABLE_COPY(RotateImageLabel)

public:
    explicit RotateImageLabel(DWidget *parent = nullptr);

public:
    void setSelect(const bool &select);
    inline void setRadius(const int radius) { m_nRadius = radius; }
    inline void setBookMarkStatus(bool bshow) { m_bshowbookmark = bshow; }

    void setBackgroundPix(QPixmap &pixmap);

    void rotateImage();

    inline void setHasThumbnail(const bool has) {m_bHasThumbnail = has;}
    inline bool hasThumbnail() {return m_bHasThumbnail;}
    void setRotateAngle(const int &angle);
    void scaleImage(const double &);//缩略图自适应窗体大小  add by duanxiaohui 2020-3-20
protected:
    void paintEvent(QPaintEvent *e) override;

private:
    bool m_bSelect = false;
    int m_nRadius = 8;             // 圆角
    bool m_bshowbookmark = false;  //显示书签

    QPixmap m_background;           // 缩略图
    QPixmap m_thumbPix;             // 显示缩略图
    bool m_bSetBp = false;          // 是否设置缩略图
    int m_nHighLightLineWidth = 0;  // 高亮边框宽度

    int m_nRotate = 0;  // 缩略图旋转度数
    bool m_bHasThumbnail = false;   // 时候加载过缩略图
    QSize m_size{0, 0}; //控件大小

    double m_scale = 1;
};

#endif  // RotateImageLabel_H

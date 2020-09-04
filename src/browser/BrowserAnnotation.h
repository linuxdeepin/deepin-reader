/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#ifndef BrowserAnnotation_H
#define BrowserAnnotation_H

#include "document/Model.h"

#include <QGraphicsItem>

const int ICON_SIZE = 23;

/**
 *文档注释类,显示类型,图元元素
 */

class BrowserPage;
class BrowserAnnotation : public QGraphicsItem
{
    friend class SheetBrowser;
public:
    explicit BrowserAnnotation(QGraphicsItem *parent, QRectF rect, deepin_reader::Annotation *annotation);

    ~BrowserAnnotation() override;

    /**
     * @brief setScaleFactorAndRotation
     * 设置注释缩放和旋转参数
     * @param rotation
     */
    void setScaleFactorAndRotation(Dr::Rotation rotation);

    /**
     * @brief annotationType
     * 注释类型
     * @return
     */
    int annotationType();

    /**
     * @brief annotationText
     * 注释内容
     * @return
     */
    QString annotationText();

    /**
     * @brief boundingRect
     * 注释大小范围(无缩放和旋转时的范围)
     * @return
     */
    QRectF boundingRect()const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    /**
     * @brief annotation
     * 注释指针
     * @return
     */
    deepin_reader::Annotation *annotation();

    /**
     * @brief deleteMe
     * 删除该注释
     */
    void deleteMe();

    /**
     * @brief isSame
     * 操作注释是否与该注释相同
     * @param annotation
     * @return
     */
    bool isSame(deepin_reader::Annotation *annotation);

    /**
     * @brief setDrawSelectRect
     * 设置选择框
     * @param draw
     */
    void setDrawSelectRect(const bool draw);

    /**
     * @brief setScaleFactor
     * 设置缩放系数
     * @param scale
     */
    void setScaleFactor(const double scale);

private:
    deepin_reader::Annotation *m_annotation;

    Dr::Rotation m_rotation = Dr::RotateBy0;

    QRectF m_rect;

    QString m_text;

    QGraphicsItem *m_parent = nullptr;

    bool m_drawSelectRect{false};

    double m_scaleFactor        = 1.0; // 弃用
};

#endif // BrowserAnnotation_H

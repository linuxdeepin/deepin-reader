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
#ifndef BrowserWord_H
#define BrowserWord_H

#include "Model.h"

#include <QGraphicsItem>
#include <QRectF>

class BrowserPage;

/**
 * @brief 用于表示一页文档中的某个文字
 */
class BrowserWord : public QGraphicsItem
{
public:
    explicit BrowserWord(QGraphicsItem *parent, deepin_reader::Word word);

    QRectF boundingRect()const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    /**
     * @brief 获取文字文本
     * @return
     */
    QString text();

    /**
     * @brief 设置文字无法选中
     * @param enable 是否可以被选中
     */
    void setSelectable(bool enable);

    /**
     * @brief 为了让文字占比固定,视图改变时需要设置缩放因子
     * @param scaleFactor 缩放因子
     */
    void setScaleFactor(qreal scaleFactor);

    /**
     * @brief BrowserWord::boundingBox
     * 文字自身的区域
     * @return
     */
    QRectF boundingBox()const;

private:
    deepin_reader::Word m_word;

    qreal m_scaleFactor = 1;

    bool m_selectable = true;
};

#endif // BrowserWord_H

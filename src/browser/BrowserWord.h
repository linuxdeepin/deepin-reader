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

#include "document/Model.h"

#include <QGraphicsItem>

class BrowserPage;

/**
 * @brief The BrowserWord class
 * 用于表示视图框架中一页文档中的某个文字
 */
class BrowserWord : public QGraphicsItem
{
public:
    explicit BrowserWord(QGraphicsItem *parent, deepin_reader::Word word);

    ~BrowserWord();

    /**
     * @brief setScaleFactor
     * 为了让文字占比固定,视图改变时需要设置缩放因子
     * @param scaleFactor 缩放因子
     */
    void setScaleFactor(qreal scaleFactor);

    /**
     * @brief text
     * 获取文字文本
     * @return
     */
    QString text();

    /**
     * @brief setSelectable
     * 设置文字是否可以被选中
     * @param enable
     */
    void setSelectable(bool enable);

    /**
     * @brief BrowserWord::boundingRect
     * 视图项的区域
     * @return
     */
    QRectF boundingRect()const override;

    /**
     * @brief BrowserWord::textBoundingRect
     * 文档文字的区域
     * @return
     */
    QRectF textBoundingRect() const;

    /**
     * @brief BrowserWord::boundingBox
     * 文字自身的区域
     * @return
     */
    QRectF boundingBox()const;

    /**
     * @brief BrowserWord::getWord
     * 获取文字
     * @return
     */
    deepin_reader::Word getWord();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

private:
    deepin_reader::Word m_word;

    qreal m_scaleFactor = 1;

    bool m_selectable = true;
};

#endif // BrowserWord_H

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

#include <QGraphicsItem>
#include "document/Model.h"

class BrowserPage;
class BrowserWord : public QGraphicsItem
{
public:
    explicit BrowserWord(QGraphicsItem *parent, deepin_reader::Word word);

    void setScaleFactor(qreal scaleFactor);

    QString text();

    bool hasSpaceAfter();

    QRectF boundingRect()const override;

    QRectF textBoundingRect() const;

    QRectF boundingBox()const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    deepin_reader::Word getWord();
private:
    deepin_reader::Word m_word;

    qreal m_scaleFactor = 1;
};

#endif // BrowserWord_H

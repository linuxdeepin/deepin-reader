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
#ifndef SHEETBROWSERPDFLANNOTATION_H
#define SHEETBROWSERPDFLANNOTATION_H

#include <QGraphicsItem>
#include "document/Model.h"

class SheetBrowserItem;
class SheetBrowserPDFLAnnotation : public QGraphicsItem
{
public:
    explicit SheetBrowserPDFLAnnotation(QGraphicsItem *parent, QRectF rect, deepin_reader::Annotation *annotation);

    void setScaleFactorAndRotation(Dr::Rotation rotation);

    int annotationType();

    QString annotationText();

    QRectF boundingRect()const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    deepin_reader::Annotation *annotation();

    void deleteMe();

private:
    deepin_reader::Annotation *m_annotation;

    Dr::Rotation m_rotation = Dr::RotateBy0;

    QRectF m_rect;

    QString m_text;

    QGraphicsItem *m_parent = nullptr;

};

#endif // SHEETBROWSERPDFLANNOTATION_H

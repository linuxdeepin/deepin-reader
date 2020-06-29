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
#ifndef SHEETBROWSERPDFLITEM_H
#define SHEETBROWSERPDFLITEM_H

#include <QGraphicsItem>
#include <QSet>

#include "Global.h"

namespace deepin_reader {
class Annotation;
class Page;
}

class SheetBrowserPDFL;
class SheetBrowserPDFLWord;
class SheetBrowserPDFLAnnotation;
class SheetBrowserPDFLItem : public QGraphicsItem
{
public:
    explicit SheetBrowserPDFLItem(SheetBrowserPDFL *parent, deepin_reader::Page *page);

    ~SheetBrowserPDFLItem() override;

    QRectF boundingRect()const override;

    QRectF bookmarkRect();

    QRectF bookmarkMouseRect();

    void setBookmark(bool hasBookmark);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

    void renderViewPort();      //优先显示当前窗口

    void render(double scaleFactor, Dr::Rotation rotation, bool renderLater = false);

    QImage getImage(double scaleFactor, Dr::Rotation rotation, const QRect &boundingRect = QRect());

    QImage getImage(int width, int height, Qt::AspectRatioMode mode); //按宽高缩放

    QImage getImageRect(double scaleFactor, QRect rect);//获取缩放后的局部区域图片

    QImage getImagePoint(double scaleFactor, QPoint point); //根据某一个点返回100×100的图片

    void handleRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image, QRect rect = QRect());

    static bool existInstance(SheetBrowserPDFLItem *item);

    void setItemIndex(int itemIndex);

    int itemIndex();

    QString selectedWords();

    void setWordSelectable(bool selectable);

    void reloadWords();

    void reloadAnnotations();

    void reload();

    QList< deepin_reader::Annotation * > annotations();

    void addHighlightAnnotation(QString text, QColor color);

    bool removeAnnotation(deepin_reader::Annotation *annotation);

protected:
    bool sceneEvent(QEvent *event) override;

signals:
    void bookmarkPressed(int, bool bookmark);

private:
    deepin_reader::Page *m_page = nullptr;

    QImage m_image;
    QImage m_leftImage;
    double m_imageScaleFactor   = 1;
    double m_scaleFactor        = -1;
    bool m_bookmark = false;
    int m_bookmarkState = 0;   //1为on 2为pressed 3为show
    int m_index = 0;
    Dr::Rotation m_rotation = Dr::RotateBy0;
    static QSet<SheetBrowserPDFLItem *> items;
    SheetBrowserPDFL *m_parent = nullptr;

    Dr::Rotation m_wordRotation = Dr::NumberOfRotations;
    QList<SheetBrowserPDFLWord *> m_words;

    Dr::Rotation m_annotationRotation = Dr::NumberOfRotations;
    double m_annotationScaleFactor = -1;
    QList<SheetBrowserPDFLAnnotation *> m_annotationItems;  //一个注释可能对应多个annotationitems
    QList<deepin_reader::Annotation *> m_annotations;

    QPointF m_posPressed;
    bool m_wordSelectable = false;
};

#endif // SHEETBROWSERPDFLITEM_H

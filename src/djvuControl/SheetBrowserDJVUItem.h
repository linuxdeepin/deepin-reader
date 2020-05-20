#ifndef SHEETBROWSERDJVUITEM_H
#define SHEETBROWSERDJVUITEM_H

#include <QGraphicsItem>
#include <QSet>

#include "global.h"

namespace deepin_reader {
class Page;
}

class SheetBrowserDJVUItem : public QGraphicsItem
{
public:
    explicit SheetBrowserDJVUItem(deepin_reader::Page *page);

    ~SheetBrowserDJVUItem();

    QRectF boundingRect()const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    void render(double scale, Dr::Rotation rotation, bool readerLater = false);

    QImage getImage(double scaleFactor, Dr::Rotation rotation);

    QImage getImageMax(double max); //获取以最大max边的缩略图

    QImage getImageRect(double scaleFactor, QRect rect);//获取缩放后的局部区域图片

    QImage getImagePoint(double scaleFactor, QPoint point); //根据某一个点返回100×100的图片

    void handleRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image);

    static bool existInstance(SheetBrowserDJVUItem *item);

private:
    deepin_reader::Page *m_page = nullptr;
    QImage m_image;
    QImage m_leftImage;
    double m_imageScaleFactor   = 1;
    double m_scaleFactor        = -1;
    Dr::Rotation m_rotation = Dr::RotateBy0;

    static QSet<SheetBrowserDJVUItem *> items;
};

#endif // SHEETBROWSERDJVUITEM_H

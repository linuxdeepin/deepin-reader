#ifndef SHEETBROWSERDJVUITEM_H
#define SHEETBROWSERDJVUITEM_H

#include <QGraphicsItem>
#include "global.h"

namespace deepin_reader {
class Page;
}

class SheetBrowserDJVUItem : public QGraphicsItem
{
public:
    explicit SheetBrowserDJVUItem(deepin_reader::Page *page);

    ~SheetBrowserDJVUItem();

    void setScale(double scale);

    void setRotation(Dr::Rotation rotation);

    QRectF boundingRect()const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    void render(double scale, Dr::Rotation rotation, bool readerLater = false);

private:
    void onReaderFinished();

private:
    deepin_reader::Page *m_page = nullptr;
    QImage m_image;
    double m_imageScaleFactor   = 1;
    double m_scaleFactor        = -1;
    Dr::Rotation m_rotation = Dr::NumberOfRotations;
};

#endif // SHEETBROWSERDJVUITEM_H

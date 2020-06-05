#ifndef SHEETBROWSERDJVUITEM_H
#define SHEETBROWSERDJVUITEM_H

#include <QGraphicsItem>
#include <QSet>

#include "global.h"

namespace deepin_reader {
class Page;
}

class SheetBrowserDJVU;
class BookMarkButton;
class SheetBrowserDJVUItem : public QGraphicsItem
{
public:
    explicit SheetBrowserDJVUItem(SheetBrowserDJVU *parent, deepin_reader::Page *page);

    ~SheetBrowserDJVUItem();

    QRectF boundingRect()const;

    QRectF bookmarkRect();

    QRectF bookmarkMouseRect();

    void setBookmark(bool hasBookmark);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    void render(double scale, Dr::Rotation rotation, bool readerLater = false);

    QImage getImage(double scaleFactor, Dr::Rotation rotation);

    QImage getImage(int width, int height, Qt::AspectRatioMode mode); //按宽高缩放

    QImage getImageRect(double scaleFactor, QRect rect);//获取缩放后的局部区域图片

    QImage getImagePoint(double scaleFactor, QPoint point); //根据某一个点返回100×100的图片

    void handleRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image);

    static bool existInstance(SheetBrowserDJVUItem *item);

    void setItemIndex(int itemIndex);

    int itemIndex();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

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
    static QSet<SheetBrowserDJVUItem *> items;
    SheetBrowserDJVU *m_parent = nullptr;
};

#endif // SHEETBROWSERDJVUITEM_H

#ifndef PAGEITEM_H
#define PAGEITEM_H

#include <QCache>
#include <QFutureWatcher>
#include <QGraphicsObject>
#include <QIcon>
#include <QSet>

#include "renderparam.h"

class ViewSettings;
class QGraphicsProxyWidget;
class ModelPage;
class TileItem;

class PageItem: public QGraphicsObject
{
    Q_OBJECT
    friend class TileItem;
public:
    enum PaintMode
    {
        DefaultMode,
        PresentationMode,
        ThumbnailMode
    };
    PageItem(ModelPage* page, int index, PaintMode paintMode = DefaultMode, QGraphicsItem* parent = 0);
    ~PageItem();
    const QRectF& uncroppedBoundingRect() const { return m_boundingRect; }
    QRectF boundingRect() const;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget*);

    QSizeF displayedSize() const { return displayedSize(renderParam()); }
    QSizeF displayedSize(const RenderParam& renderParam) const;
    const RenderParam& renderParam() const { return m_renderParam; }
    void setRenderParam(const RenderParam& renderParam);

public slots:
    void refresh(bool keepObsoletePixmaps = false, bool dropCachedPixmaps = false);
    int startRender(bool prefetch = false);
    void cancelRender();

private:
     // geometry
    void prepareGeometry();
    void prepareTiling();


    // paint
    void paintPage(QPainter* painter, const QRectF& exposedRect) const;

     bool useTiling() const;

private:
    Q_DISABLE_COPY(PageItem)
    static ViewSettings* s_settings;
    ModelPage* m_page;
    QSizeF m_size;
    PaintMode m_paintMode;
    RenderParam m_renderParam;
    int m_index;
    QTransform m_transform;
    QTransform m_normalizedTransform;
    QRectF m_boundingRect;
    QRectF m_cropRect;
    QVector< TileItem* > m_tileItems;
    mutable QSet< TileItem* > m_exposedTileItems;

};

#endif // PAGEITEM_H

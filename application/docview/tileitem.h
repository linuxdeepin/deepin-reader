#ifndef TILEITEM_H
#define TILEITEM_H
#include <QCache>
#include <QObject>
#include <QPixmap>

#include "rendertask.h"

class PageItem;

class TileItem : public RenderTaskParent
{
public:
    TileItem(PageItem* page);
    ~TileItem();

    const QRect& rect() const { return m_rect; }
    void setRect(QRect rect) { m_rect = rect; }

    const QRectF& cropRect() const { return m_cropRect; }
     void resetCropRect() { m_cropRect = QRectF(); }
     void setCropRect(const QRectF& cropRect);

    void dropPixmap() { m_pixmap = QPixmap(); }
    void dropObsoletePixmap() { m_obsoletePixmap = QPixmap(); }

    static void dropCachedPixmaps(PageItem* page);

    bool paint(QPainter* painter, QPointF topLeft);

public:
    void refresh(bool keepObsoletePixmaps = false);

    int startRender(bool prefetch = false);
    void cancelRender();

    void deleteAfterRender();

private:
    void on_finished(const RenderParam& renderParam,
                     const QRect& rect, bool prefetch,
                     const QImage& image, const QRectF& cropRect);
    void on_canceled();
    void on_finishedOrCanceled();

private:
    Q_DISABLE_COPY(TileItem)

    static ViewSettings* s_settings;

    typedef QPair< PageItem*, QByteArray > CacheKey;
    typedef QPair< QPixmap, QRectF > CacheObject;

    static QCache< CacheKey, CacheObject > s_cache;

    CacheKey cacheKey() const;

    PageItem* m_page;

    QRect m_rect;
    QRectF m_cropRect;

    bool m_pixmapError;
    QPixmap m_pixmap;
    QPixmap m_obsoletePixmap;

    QPixmap takePixmap();

    bool m_deleteAfterRender;
    RenderTask m_renderTask;
};

#endif // TILEITEM_H

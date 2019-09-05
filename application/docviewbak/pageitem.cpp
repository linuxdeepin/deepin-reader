#include <QStyleOptionGraphicsItem>
#include <QPainter>
#include <QtMath>

#include "model.h"
#include "pageitem.h"
#include "viewsettings.h"
#include "tileitem.h"

const int largeTilesThreshold = 8;
const int veryLargeTilesThreshold = 16;


ViewSettings* PageItem::s_settings=nullptr;
PageItem::PageItem(ModelPage *page, int index, PageItem::PaintMode paintMode, QGraphicsItem *parent): QGraphicsObject(parent),
    m_page(page),
    m_size(page->size()),
    m_index(index),
    m_paintMode(paintMode),
    m_transform(),
    m_normalizedTransform(),
    m_boundingRect(),
    m_renderParam(),
    m_tileItems()
{
    if(s_settings == 0)
    {
        s_settings = ViewSettings::instance();
    }

    if(!useTiling())
    {
        m_tileItems.resize(1);
        m_tileItems.squeeze();

        m_tileItems.replace(0, new TileItem(this));
    }
    prepareGeometry();
}

PageItem::~PageItem()
{
    TileItem::dropCachedPixmaps(this);
    qDeleteAll(m_tileItems);
}

QRectF PageItem::boundingRect() const
{
    if(m_cropRect.isNull())
    {
        return m_boundingRect;
    }

    QRectF boundingRect;

    boundingRect.setLeft(m_boundingRect.left() + m_cropRect.left() * m_boundingRect.width());
    boundingRect.setTop(m_boundingRect.top() + m_cropRect.top() * m_boundingRect.height());
    boundingRect.setWidth(m_cropRect.width() * m_boundingRect.width());
    boundingRect.setHeight(m_cropRect.height() * m_boundingRect.height());

    return boundingRect;
}

void PageItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    paintPage(painter, option->exposedRect);
    //TODO HightLight Link
}

QSizeF PageItem::displayedSize(const RenderParam &renderParam) const
{
    const bool rotationChanged = m_renderParam.rotation() != renderParam.rotation();

    const bool flagsChanged = m_renderParam.flags() != renderParam.flags();

    const bool useCropRect = !m_cropRect.isNull() && !rotationChanged && !flagsChanged;

    const qreal cropWidth = useCropRect ? m_cropRect.width() : 1.0;
    const qreal cropHeight = useCropRect ? m_cropRect.height() : 1.0;

    switch(renderParam.rotation())
    {
    default:
    case RotateBy0:
    case RotateBy180:
        return QSizeF(renderParam.resolutionX() / 72.0 * cropWidth * m_size.width(),
                      renderParam.resolutionY() / 72.0 * cropHeight * m_size.height());
    case RotateBy90:
    case RotateBy270:
        return QSizeF(renderParam.resolutionX() / 72.0 * cropHeight * m_size.height(),
                      renderParam.resolutionY() / 72.0 * cropWidth * m_size.width());
    }
}

void PageItem::setRenderParam(const RenderParam &renderParam)
{
    if(m_renderParam != renderParam)
    {
        const bool resolutionChanged = m_renderParam.resolutionX() != renderParam.resolutionX()
                || m_renderParam.resolutionY() != renderParam.resolutionY()
                || !qFuzzyCompare(m_renderParam.devicePixelRatio(), renderParam.devicePixelRatio())
                || !qFuzzyCompare(m_renderParam.scaleFactor(), renderParam.scaleFactor());

        const bool rotationChanged = m_renderParam.rotation() != renderParam.rotation();

        const RenderPaperFlags changedFlags = m_renderParam.flags() ^ renderParam.flags();

        refresh(!rotationChanged && changedFlags == 0);

        m_renderParam = renderParam;

        if(resolutionChanged || rotationChanged)
        {
            prepareGeometryChange();
            prepareGeometry();
        }

        if(changedFlags.testFlag(TrimMargins))
        {
            setFlag(QGraphicsItem::ItemClipsToShape, m_renderParam.trimMargins());
        }
    }
}

void PageItem::refresh(bool keepObsoletePixmaps, bool dropCachedPixmaps)
{
    if(!keepObsoletePixmaps)
    {
        prepareGeometryChange();

        m_cropRect = QRectF();
    }
}

int PageItem::startRender(bool prefetch)
{
    int cost = 0;

    if(!useTiling())
    {
        cost += m_tileItems.first()->startRender(prefetch);
    }
    else
    {
        foreach(TileItem* tile, m_tileItems)
        {
            cost += tile->startRender(prefetch);
        }
    }

    return cost;
}

void PageItem::cancelRender()
{
    if(!useTiling())
    {
        m_tileItems.first()->cancelRender();
    }
    else
    {
        foreach(TileItem* tile, m_exposedTileItems)
        {
            tile->cancelRender();
        }

        m_exposedTileItems.clear();
    }
}

void PageItem::prepareGeometry()
{
    m_transform.reset();

    m_transform.scale(m_renderParam.resolutionX() * m_renderParam.scaleFactor() / 72.0,
                      m_renderParam.resolutionY() * m_renderParam.scaleFactor() / 72.0);

    switch(m_renderParam.rotation())
    {
    default:
    case RotateBy0:
        break;
    case RotateBy90:
        m_transform.rotate(90.0);
        break;
    case RotateBy180:
        m_transform.rotate(180.0);
        break;
    case RotateBy270:
        m_transform.rotate(270.0);
        break;
    }

    m_normalizedTransform = m_transform;
    m_normalizedTransform.scale(m_size.width(), m_size.height());

    m_boundingRect = m_transform.mapRect(QRectF(QPointF(), m_size));

    m_boundingRect.setWidth(qRound(m_boundingRect.width()));
    m_boundingRect.setHeight(qRound(m_boundingRect.height()));
     prepareTiling();
}

void PageItem::prepareTiling()
{
    if(!useTiling())
    {
        m_tileItems.first()->setRect(QRect(0, 0, m_boundingRect.width(), m_boundingRect.height()));

        return;
    }


    const qreal pageWidth = m_boundingRect.width();
    const qreal pageHeight = m_boundingRect.height();
    const qreal pageSize = qMax(pageWidth, pageHeight);

    int tileSize = s_settings->pageItem().tileSize();

    if(tileSize * veryLargeTilesThreshold < pageSize)
    {
        tileSize *= 4;
    }
    else if(tileSize * largeTilesThreshold < pageSize)
    {
        tileSize *= 2;
    }

    int tileWidth = pageWidth < pageHeight ? tileSize * pageWidth / pageHeight : tileSize;
    int tileHeight = pageHeight < pageWidth ? tileSize * pageHeight / pageWidth : tileSize;

    const int columnCount = qCeil(pageWidth / tileWidth);
    const int rowCount = qCeil(pageHeight / tileHeight);

    tileWidth = qCeil(pageWidth / columnCount);
    tileHeight = qCeil(pageHeight / rowCount);


    const int newCount = columnCount * rowCount;
    const int oldCount = m_tileItems.count();

    if(oldCount != newCount)
    {
        for(int index = newCount; index < oldCount; ++index)
        {
            m_tileItems.at(index)->deleteAfterRender();
        }

        m_tileItems.resize(newCount);

        for(int index = oldCount; index < newCount; ++index)
        {
            m_tileItems.replace(index, new TileItem(this));
        }

        foreach(TileItem* tile, m_tileItems)
        {
            tile->dropObsoletePixmap();
        }
    }

    m_exposedTileItems.clear();


    for(int column = 0; column < columnCount; ++column)
    {
        for(int row = 0; row < rowCount; ++row)
        {
            const int left = column > 0 ? column * tileWidth : 0.0;
            const int top = row > 0 ? row * tileHeight : 0.0;

            const int width = column < (columnCount - 1) ? tileWidth : pageWidth - left;
            const int height = row < (rowCount - 1) ? tileHeight : pageHeight - top;

            m_tileItems.at(column * rowCount + row)->setRect(QRect(left, top, width, height));
        }
    }
}

void PageItem::paintPage(QPainter *painter, const QRectF &exposedRect) const
{

    if(s_settings->pageItem().decoratePages()/* && !presentationMode()*/)
    {
        // background

        QColor paperColor = s_settings->pageItem().paperColor();

        if(m_renderParam.invertColors())
        {
            paperColor.setRgb(~paperColor.rgb());
        }

        painter->fillRect(m_boundingRect, QBrush(paperColor));
    }
    {
        const QRectF& translatedExposedRect = exposedRect.translated(-m_boundingRect.topLeft());

        foreach(TileItem* tile, m_tileItems)
        {
            const bool intersects = translatedExposedRect.intersects(tile->rect());
            const bool contains = m_exposedTileItems.contains(tile);

            if(intersects && !contains)
            {
                m_exposedTileItems.insert(tile);
            }
            else if(!intersects && contains)
            {
                m_exposedTileItems.remove(tile);

                tile->cancelRender();
            }
        }

        bool allExposedPainted = true;

        foreach(TileItem* tile, m_exposedTileItems)
        {
            if(!tile->paint(painter, m_boundingRect.topLeft()))
            {
                allExposedPainted = false;
            }
        }

        if(allExposedPainted)
        {
            foreach(TileItem* tile, m_exposedTileItems)
            {
                tile->dropPixmap();
            }
        }
    }
    // border

    painter->save();

    painter->setClipping(false);

    painter->drawRect(m_renderParam.trimMargins() ? PageItem::boundingRect() : PageItem::uncroppedBoundingRect());

    painter->restore();

}

bool PageItem::useTiling() const
{
    return m_paintMode != ThumbnailMode && s_settings->pageItem().useTiling();
}

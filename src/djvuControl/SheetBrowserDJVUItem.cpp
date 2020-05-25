#include "SheetBrowserDJVUItem.h"
#include "document/model.h"
#include "RenderThreadDJVU.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>
#include <QGraphicsSceneMouseEvent>

QSet<SheetBrowserDJVUItem *> SheetBrowserDJVUItem::items;
SheetBrowserDJVUItem::SheetBrowserDJVUItem(deepin_reader::Page *page) : QGraphicsItem()
{
    m_page = page;
    items.insert(this);
}

SheetBrowserDJVUItem::~SheetBrowserDJVUItem()
{
    items.remove(this);
    if (nullptr != m_page)
        delete m_page;
}

QRectF SheetBrowserDJVUItem::boundingRect() const
{
    if (nullptr == m_page)
        return QRectF(0, 0, 0, 0);

    switch (m_rotation) {
    case Dr::RotateBy90:
    case Dr::RotateBy270:
        return QRectF(0, 0, (double)m_page->size().height() * m_scaleFactor, (double)m_page->size().width() * m_scaleFactor);
    default: break;
    }

    return QRectF(0, 0, (double)m_page->size().width() * m_scaleFactor, (double)m_page->size().height() * m_scaleFactor);
}

QRectF SheetBrowserDJVUItem::bookmarkRect()
{
    return QRectF(boundingRect().width() - 40, 0, 40, 40);
}

void SheetBrowserDJVUItem::setBookmark(bool hasBookmark)
{
    m_bookmark = hasBookmark;
    if (hasBookmark)
        m_bookmarkState = 3;
    else
        m_bookmarkState = 0;
    update();
}

void SheetBrowserDJVUItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (m_image.isNull() || !qFuzzyCompare(m_imageScaleFactor, m_scaleFactor)) {
        render(m_scaleFactor, m_rotation, false);
    }

    if (m_image.isNull())
        painter->drawImage(option->rect, m_leftImage);
    else
        painter->drawImage(option->rect.x(), option->rect.y(), m_image);

    if (1 == m_bookmarkState)
        painter->drawPixmap(QRect(bookmarkRect().x(), bookmarkRect().y(), bookmarkRect().width(), bookmarkRect().height()), QPixmap(":/custom/bookmark_hover.svg"));
    if (2 == m_bookmarkState)
        painter->drawPixmap(QRect(bookmarkRect().x(), bookmarkRect().y(), bookmarkRect().width(), bookmarkRect().height()), QPixmap(":/custom/bookmark_pressed.svg"));
    if (3 == m_bookmarkState)
        painter->drawPixmap(QRect(bookmarkRect().x(), bookmarkRect().y(), bookmarkRect().width(), bookmarkRect().height()), QPixmap(":/custom/bookmark.svg"));
}

void SheetBrowserDJVUItem::render(double scaleFactor, Dr::Rotation rotation, bool readerLater)
{
    if (nullptr == m_page)
        return;

    if (!m_image.isNull() && qFuzzyCompare(scaleFactor, m_imageScaleFactor) && rotation == m_rotation)
        return;

    m_scaleFactor = scaleFactor;

    if (!m_image.isNull())
        m_leftImage = m_image;

    if (m_rotation != rotation) {//旋转变化则强制移除
        m_leftImage = QImage();
        m_rotation  = rotation;
    }

    if (!readerLater) {
        if (m_leftImage.isNull())
            m_leftImage = getImage(0.1, rotation);

        //之后替换为线程reader
        RenderThreadDJVU::appendTask(this, m_scaleFactor, m_rotation);
        m_imageScaleFactor = m_scaleFactor;
    }

    m_image = QImage();

    update();
}

QImage SheetBrowserDJVUItem::getImage(double scaleFactor, Dr::Rotation rotation)
{
    return m_page->render(rotation, scaleFactor);
}

QImage SheetBrowserDJVUItem::getImage(int width, int height, Qt::AspectRatioMode mode)
{
    return m_page->render(width, height, mode);
}

QImage SheetBrowserDJVUItem::getImageMax(double max)
{
    qreal scaleFactor = max / qMax(m_page->size().height(), m_page->size().width());
    return m_page->render(m_rotation, scaleFactor);
}

QImage SheetBrowserDJVUItem::getImageRect(double scaleFactor, QRect rect)
{
    return m_page->render(m_rotation, scaleFactor, rect);
}

QImage SheetBrowserDJVUItem::getImagePoint(double scaleFactor, QPoint point)
{
    QRect rect = QRect(point.x() * scaleFactor / m_scaleFactor - 117, point .y() * scaleFactor / m_scaleFactor  - 117, 234, 234);

    return m_page->render(m_rotation, scaleFactor, rect);
}

void SheetBrowserDJVUItem::handleRenderFinished(double scaleFactor, Dr::Rotation rotation, QImage image)
{
    m_image = image;
    update();
}

bool SheetBrowserDJVUItem::existInstance(SheetBrowserDJVUItem *item)
{
    return items.contains(item);
}

void SheetBrowserDJVUItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (bookmarkRect().contains(event->pos())) {
        m_bookmarkState = 2;
        update();
    }

    QGraphicsItem::mousePressEvent(event);
}

void SheetBrowserDJVUItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (!m_bookmark && bookmarkRect().contains(event->pos())) {
        m_bookmarkState = 1;
        update();
    } else if (m_bookmark)
        m_bookmarkState = 3;

    update();

    QGraphicsItem::mouseMoveEvent(event);
}

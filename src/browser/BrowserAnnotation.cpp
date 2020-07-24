#include "BrowserAnnotation.h"
#include "BrowserPage.h"
#include "document/Model.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

BrowserAnnotation::BrowserAnnotation(QGraphicsItem *parent, QRectF rect, deepin_reader::Annotation *annotation) : QGraphicsItem(parent),
    m_annotation(annotation), m_rect(rect), m_parent(parent)
{
    if (m_annotation->type() == 1/*TextIcon*/)
        setZValue(deepin_reader::Z_ORDER_ICON);
    else
        setZValue(deepin_reader::Z_ORDER_HIGHLIGHT);
}

void BrowserAnnotation::setScaleFactorAndRotation(Dr::Rotation rotation)
{
    m_rotation = rotation;
    update();
}

int BrowserAnnotation::annotationType()
{
    return m_annotation->type();
}

QString BrowserAnnotation::annotationText()
{
    return m_annotation->contents();
}

QRectF BrowserAnnotation::boundingRect() const
{
    return QRectF(m_rect.x() * m_parent->boundingRect().width(),
                  m_rect.y() * m_parent->boundingRect().height(),
                  m_rect.width() * m_parent->boundingRect().width(),
                  m_rect.height() * m_parent->boundingRect().height());
}

void BrowserAnnotation::paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *)
{
    //notTodo
}

deepin_reader::Annotation *BrowserAnnotation::annotation()
{
    return m_annotation;
}

void BrowserAnnotation::deleteMe()
{
    BrowserPage *item = static_cast<BrowserPage *>(m_parent);
    if (nullptr == item)
        return;

    item->removeAnnotation(m_annotation);
}

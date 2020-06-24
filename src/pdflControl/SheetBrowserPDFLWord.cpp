#include "SheetBrowserPDFLWord.h"
#include "SheetBrowserPDFLItem.h"
#include "document/Model.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

SheetBrowserPDFLWord::SheetBrowserPDFLWord(QGraphicsItem *parent, deepin_reader::Word word) : QGraphicsItem(parent), m_word(word)
{
    setAcceptHoverEvents(true);
    setFlag(QGraphicsItem::ItemIsSelectable);
}

void SheetBrowserPDFLWord::setScaleFactor(qreal scaleFactor)
{
    m_scaleFactor = scaleFactor;
    update();
}

QString SheetBrowserPDFLWord::text()
{
    return m_word.text;
}

QRectF SheetBrowserPDFLWord::boundingRect() const
{
    return QRectF(m_word.boundingBox.x() * m_scaleFactor - 1, m_word.boundingBox.y() * m_scaleFactor - 1, m_word.boundingBox.width() * m_scaleFactor + 2, m_word.boundingBox.height() * m_scaleFactor + 2);
}

void SheetBrowserPDFLWord::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (isSelected()) {
        painter->setBrush(QBrush(Qt::red));
        painter->setOpacity(0.1);
        painter->drawRect(option->rect.x() + 1, option->rect.y() + 1, option->rect.width() - 2, option->rect.height() - 2);
    }
}

//void SheetBrowserPDFLWord::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
//{
//    setCursor(QCursor(Qt::IBeamCursor));
//}

//void SheetBrowserPDFLWord::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
//{
//    setCursor(QCursor(Qt::ArrowCursor));
//}

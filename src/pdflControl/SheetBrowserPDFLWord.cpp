#include "SheetBrowserPDFLWord.h"
#include "SheetBrowserPDFLItem.h"
#include "document/Model.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

SheetBrowserPDFLWord::SheetBrowserPDFLWord(QGraphicsItem *parent, deepin_reader::Word word) : QGraphicsItem(parent), m_word(word)
{

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
    return QRectF(m_word.boundingBox.x() * m_scaleFactor, m_word.boundingBox.y() * m_scaleFactor, m_word.boundingBox.width() * m_scaleFactor, m_word.boundingBox.height() * m_scaleFactor);
}

void SheetBrowserPDFLWord::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setBrush(QBrush(Qt::red));
    painter->setOpacity(0.1);
    painter->drawRect(option->rect);
}

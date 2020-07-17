#include "BrowserWord.h"
#include "BrowserPage.h"
#include "document/Model.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QDebug>

BrowserWord::BrowserWord(QGraphicsItem *parent, deepin_reader::Word word) : QGraphicsItem(parent), m_word(word)
{

}

void BrowserWord::setScaleFactor(qreal scaleFactor)
{
    m_scaleFactor = scaleFactor;
    update();
}

QString BrowserWord::text()
{
    return m_word.text;
}

QRectF BrowserWord::boundingRect() const
{
    return QRectF(m_word.boundingBox.x() * m_scaleFactor - 1, m_word.boundingBox.y() * m_scaleFactor - 1, m_word.boundingBox.width() * m_scaleFactor + 2, m_word.boundingBox.height() * m_scaleFactor + 2);
}

/**
 * @brief BrowserWord::textBoundingRect
 *文档文字的当前的rect
 * @return
 */
QRectF BrowserWord::textBoundingRect() const
{
    return QRectF(m_word.boundingBox.x(), m_word.boundingBox.y(),
                  m_word.boundingBox.width(), m_word.boundingBox.height());
}

QRectF BrowserWord::boundingBox() const
{
    return m_word.boundingBox;
}

void BrowserWord::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
    if (isSelected()) {
        painter->setBrush(QBrush(Qt::red));
        painter->setPen(Qt::NoPen);
        painter->setOpacity(0.1);
        painter->drawRect(option->rect.x() + 1, option->rect.y() + 1, option->rect.width() - 2, option->rect.height() - 2);
    }
}

Word BrowserWord::getWord()
{
    return m_word;
}

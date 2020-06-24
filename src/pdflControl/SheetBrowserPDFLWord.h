#ifndef SHEETBROWSERPDFLWORD_H
#define SHEETBROWSERPDFLWORD_H

#include <QGraphicsItem>
#include "document/Model.h"

class SheetBrowserItem;
class SheetBrowserPDFLWord : public QGraphicsItem
{
public:
    explicit SheetBrowserPDFLWord(QGraphicsItem *parent, deepin_reader::Word word);

    void setScaleFactor(qreal scaleFactor);

    QString text();

    QRectF boundingRect()const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

//    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);

//    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    deepin_reader::Word m_word;

    qreal m_scaleFactor = 1;
};

#endif // SHEETBROWSERPDFLWORD_H

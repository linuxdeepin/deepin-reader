#ifndef BROWSERLINK_H
#define BROWSERLINK_H

#include <QGraphicsItem>
#include "document/Model.h"

class BrowserLink : public QGraphicsItem
{
public:
    explicit BrowserLink(QGraphicsItem *parent, deepin_reader::Link *link);

    QRectF boundingRect()const override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

};

#endif // BROWSERLINK_H

#ifndef MYLABEL_H
#define MYLABEL_H

#include <QLabel>
#include <DWidget>
#include <QPainter>
#include <QRectF>
#include <QString>

class MyLabel : public QLabel
{
    Q_OBJECT
public:
    MyLabel(QWidget *parent = 0);
    ~MyLabel() override;

protected:
    void  paintEvent(QPaintEvent *e) override;
};

#endif // MYLABEL_H

#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QLabel>
#include <DWidget>
#include <QPainter>
#include <QRectF>
#include <QString>
#include <QPainterPath>

class CustomLabel : public QLabel
{
    Q_OBJECT
public:
    CustomLabel(QWidget *parent = nullptr);
    ~CustomLabel() override;

protected:
    void  paintEvent(QPaintEvent *e) override;
};

#endif // CUSTOMLABEL_H

#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <QLabel>
#include <DWidget>
#include <QPainter>
#include <QRectF>
#include <QString>
#include <QPainterPath>

/**
 * @brief The ThumbnailItemWidget class
 * @brief   存放缩略图Label
 */

class CustomLabel : public QLabel
{
    Q_OBJECT
public:
    CustomLabel(QWidget *parent = nullptr);

protected:
    void  paintEvent(QPaintEvent *e) override;
};

#endif // CUSTOMLABEL_H

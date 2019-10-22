#ifndef CUSTOMLABEL_H
#define CUSTOMLABEL_H

#include <DWidget>
#include <DLabel>

#include <QPainter>
#include <QRectF>
#include <QString>
#include <QPainterPath>
#include <QLabel>

DWIDGET_USE_NAMESPACE

/**
 * @brief The ThumbnailItemWidget class
 * @brief   存放缩略图Label
 */

class CustomLabel : public DLabel
{
    Q_OBJECT
public:
    CustomLabel(QWidget *parent = nullptr);

protected:
    void  paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
};

#endif // CUSTOMLABEL_H

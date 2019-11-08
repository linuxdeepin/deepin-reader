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

public:
    void setSelect(const bool select);

protected:
    void  paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;

private:
    bool m_bSelect = false;       //  是否被选中
};

#endif // CUSTOMLABEL_H

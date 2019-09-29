#ifndef THUMBNAILITEMWIDGET_H
#define THUMBNAILITEMWIDGET_H


#include <DLabel>
#include <QVBoxLayout>
#include <QPainter>
#include <QImage>

#include "CustomLabel.h"
#include "CustomItemWidget.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   缩略图中的item
 */

class ThumbnailItemWidget  : public CustomItemWidget
{
    Q_OBJECT
public:
    ThumbnailItemWidget(CustomItemWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void setBSelect(const bool &paint);

protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    bool m_bPaint = false;                    // 是否绘制选中item
};

#endif // THUMBNAILITEMWIDGET_H

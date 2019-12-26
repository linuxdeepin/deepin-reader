#ifndef THUMBNAILITEMWIDGET_H
#define THUMBNAILITEMWIDGET_H

#include <DLabel>
#include <QVBoxLayout>

#include "CustomItemWidget.h"

/**
 * @brief The ThumbnailItemWidget class
 * @brief   缩略图中的item
 */

class ThumbnailItemWidget : public CustomItemWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(ThumbnailItemWidget)

public:
    explicit ThumbnailItemWidget(DWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void setBSelect(const bool &paint);
    void rotateThumbnail(int);

signals:
    void sigBookMarkStatusChanged(bool);
public slots:
    void slotBookMarkShowStatus(bool bshow);

protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    bool m_bPaint = false;  // 是否绘制选中item

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
};

#endif  // THUMBNAILITEMWIDGET_H

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
    void paintEvent(QPaintEvent *event) override;

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;
    void setPageLabelText(const QString &);

    void setBSelect(const bool &paint);

protected:
//    void paintEvent(QPaintEvent *event) override;
    void initWidget() override;

private:
    DLabel *m_pPageLabel = nullptr;
    bool m_bPaint = false;
};

#endif // THUMBNAILITEMWIDGET_H

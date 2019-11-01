#ifndef BOOKMARKITEMWIDGET_H
#define BOOKMARKITEMWIDGET_H

#include <DLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "CustomItemWidget.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签列表item
 */


class BookMarkItemWidget : public CustomItemWidget
{
    Q_OBJECT
public:
    BookMarkItemWidget(CustomItemWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    inline void setBSelect(const bool &paint)
    {
        m_bPaint = paint;
        update();
    }

private slots:
    void slotDltBookMark();
    void slotShowContextMenu(const QPoint &);

protected:
    void initWidget() Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_bPaint = false;                    // 是否绘制选中item
};

#endif // BOOKMARKITEMWIDGET_H

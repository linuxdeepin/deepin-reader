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
    Q_DISABLE_COPY(BookMarkItemWidget)

public:
    explicit BookMarkItemWidget(QWidget *parent = nullptr);

signals:
    void sigDeleleteItem(const int &);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    bool bSelect();
    void setBSelect(const bool &paint);

private slots:
    void slotDltBookMark();
    void slotShowContextMenu(const QPoint &);

protected:
    void initWidget() Q_DECL_OVERRIDE;
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

private:
    bool m_bPaint = false;                    // 是否绘制选中item
    DMenu *m_menu = nullptr;

    // QWidget interface
protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;
};

#endif // BOOKMARKITEMWIDGET_H

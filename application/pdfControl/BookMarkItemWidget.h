#ifndef BOOKMARKITEMWIDGET_H
#define BOOKMARKITEMWIDGET_H

#include <DLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "translator/Frame.h"
#include "CustomItemWidget.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签列表item
 */

//操作类型
enum OPERATION_TYPE {
    ADDITEM = 0,        //增加item
    DLTITEM,            //删除item
};

class BookMarkItemWidget : public CustomItemWidget
{
    Q_OBJECT
public:
    BookMarkItemWidget(CustomItemWidget *parent = nullptr);

public:
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

private slots:
    void slotDltBookMark();
    void slotShowContextMenu(const QPoint &);

protected:
    void initWidget() override;
};

#endif // BOOKMARKITEMWIDGET_H

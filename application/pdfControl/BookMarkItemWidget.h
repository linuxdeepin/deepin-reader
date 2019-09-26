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

    inline DLabel *pageLabel() const
    {
        return m_pPage;
    }

    int PageNumber() const;
    void setPageNumber(int nPageNumber);

private slots:
    void slotDltBookMark();
    void slotShowContextMenu(const QPoint &);

protected:
    void initWidget() override;

private:
    int     m_nPageNumber = -1;
    DLabel *m_pPage = nullptr;
};

#endif // BOOKMARKITEMWIDGET_H

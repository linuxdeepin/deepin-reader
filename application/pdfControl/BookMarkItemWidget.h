#ifndef BOOKMARKITEMWIDGET_H
#define BOOKMARKITEMWIDGET_H

#include <DLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "translator/BookMark.h"
#include "subjectObserver/CustomWidget.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签列表item
 */

using namespace  BOOKMARK;

//操作类型
enum OPERATION_TYPE {
    ADDITEM = 0,        //增加item
    DLTITEM,            //删除item
};

class BookMarkItemWidget : public CustomWidget
{
    Q_OBJECT
public:
    BookMarkItemWidget(CustomWidget *parent = nullptr);

public:
    void setItemImage(const QImage &);
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
    DLabel *m_pPicture = nullptr;
    DLabel *m_pPage = nullptr;
    QHBoxLayout *m_pHLayout = nullptr;
};

#endif // BOOKMARKITEMWIDGET_H

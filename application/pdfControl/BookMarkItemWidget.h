#ifndef BOOKMARKITEMWIDGET_H
#define BOOKMARKITEMWIDGET_H

#include <DLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "subjectObserver/CustomWidget.h"

/**
 * @brief The BookMarkWidget class
 * @brief   书签列表item
 */

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
    void setPage(const QString &);
    // IObserver interface
    int dealWithData(const int &, const QString &) override;

    inline DLabel *pageLabel() const
    {
        return m_pPage;
    }

    void destructMember();

private slots:
    void slotDltBookMark();
    void slotShowContextMenu(const QPoint &);

protected:
    void initWidget() override;

private:
    DLabel *m_pPicture = nullptr;
    DLabel *m_pPage = nullptr;
    QHBoxLayout *m_pHLayout = nullptr;
};

#endif // BOOKMARKITEMWIDGET_H

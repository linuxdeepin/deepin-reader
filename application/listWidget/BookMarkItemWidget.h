#ifndef BOOKMARKITEMWIDGET_H
#define BOOKMARKITEMWIDGET_H

#include <DWidget>
#include <DLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

class BookMarkItemWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT
public:
    BookMarkItemWidget(DWidget *parent = nullptr);
    ~BookMarkItemWidget() override;

public:
    void setPicture(const QString &);
    void setPage(const QString &);
    // IObserver interface
    int update(const int &, const QString &) override;

private slots:
    void slotDltBookMark();
    void slotShowContextMenu(const QPoint &);

private:
    void initWidget();

private:
    DLabel *m_pPicture = nullptr;
    DLabel *m_pPage = nullptr;
    QHBoxLayout *m_pHLayout = nullptr;
    MsgSubject    *m_pMsgSubject = nullptr;
};

#endif // BOOKMARKITEMWIDGET_H

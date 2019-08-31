#ifndef BOOKMARKITEMWIDGET_H
#define BOOKMARKITEMWIDGET_H

#include <DWidget>
#include <DLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

class BookMarkItemWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT
public:
    BookMarkItemWidget(DWidget *parent = nullptr);
    ~BookMarkItemWidget() override;

public:
    void setPicture(const QString&);
    void setPage(const QString &);
    // IObserver interface
    int update(const QString &) override;

private slots:
    void slotDltBookMark();
    void slotShowContextMenu(const QPoint&);

private:
    void initWidget();

private:
    DLabel * m_pPicture = nullptr;
    DLabel * m_pPage = nullptr;
    QHBoxLayout * m_pHLayout = nullptr;
    ThemeSubject    *m_pThemeSubject = nullptr;
};

#endif // BOOKMARKITEMWIDGET_H

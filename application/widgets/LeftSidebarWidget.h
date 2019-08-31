#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include <DWidget>
#include <QVBoxLayout>
#include <DStackedWidget>
#include <QEvent>

#include "MainOperationWidget.h"
#include "listWidget/ThumbnailWidget.h"
#include "listWidget/BookMarkWidget.h"
#include "listWidget/NotesWidget.h"

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"
#include "header/MsgHeader.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The LeftShowWidget class
 * @brief   侧边栏显示
 */

enum WIDGETINDEX{
    THUMBNAIL = 0,
    BOOK,
    NOTE
};

class LeftSidebarWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT
public:
    LeftSidebarWidget(DWidget *parent = nullptr);
    ~LeftSidebarWidget() override;

private:
    void initOperationWidget();

private:
    QVBoxLayout             *m_pVBoxLayout = nullptr;
    DStackedWidget          *m_pStackedWidget = nullptr;
    MainOperationWidget     *m_operationWidget = nullptr;

    ThumbnailWidget * m_pThumbnailWidget = nullptr;
    BookMarkWidget * m_pBookMarkWidget = nullptr;
    NotesWidget * m_pNotesWidget = nullptr;

    MsgSubject            *m_pMsgSubject = nullptr;

    // IObserver interface
public:
    int update(const int&, const QString &) override;
};

#endif // LEFTSHOWWIDGET_H

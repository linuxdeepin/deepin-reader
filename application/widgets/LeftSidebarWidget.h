#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H

#include <DStackedWidget>
#include <DWidget>
#include <QEvent>
#include <QVBoxLayout>

#include "MainOperationWidget.h"
#include "listWidget/BookMarkWidget.h"
#include "listWidget/NotesWidget.h"
#include "listWidget/ThumbnailWidget.h"

#include "header/CustomWidget.h"
#include "header/MsgHeader.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The LeftShowWidget class
 * @brief   侧边栏显示
 */

enum WIDGETINDEX {
    THUMBNAIL = 0,
    BOOK,
    NOTE
};

class LeftSidebarWidget : public CustomWidget
{
    Q_OBJECT
public:
    LeftSidebarWidget(CustomWidget *parent = nullptr);

private:
    void initOperationWidget();

private:
    QVBoxLayout *m_pVBoxLayout = nullptr;
    DStackedWidget *m_pStackedWidget = nullptr;
    MainOperationWidget *m_operationWidget = nullptr;

    ThumbnailWidget *m_pThumbnailWidget = nullptr;
    BookMarkWidget *m_pBookMarkWidget = nullptr;
    NotesWidget *m_pNotesWidget = nullptr;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int update(const int &, const QString &) override;
};

#endif // LEFTSHOWWIDGET_H

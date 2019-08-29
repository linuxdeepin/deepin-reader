#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H


#include <DWidget>
#include <QVBoxLayout>
#include <DStackedWidget>
#include "MainOperationWidget.h"

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The LeftShowWidget class
 * @brief   侧边栏显示
 */


class LeftSidebarWidget : public DWidget, public IThemeObserver
{
public:
    LeftSidebarWidget(DWidget *parent = nullptr);
    ~LeftSidebarWidget();

private:
    void initOperationWidget();

private:
    QVBoxLayout             *m_pVBoxLayout = nullptr;

    DStackedWidget          *m_pStackedWidget = nullptr;
    MainOperationWidget     *m_operationWidget = nullptr;

    ThemeSubject            *m_pThemeSubject = nullptr;

    // IObserver interface
public:
    int update(const QString &);
};

#endif // LEFTSHOWWIDGET_H

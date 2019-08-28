#ifndef LEFTSIDEBARWIDGET_H
#define LEFTSIDEBARWIDGET_H


#include <DWidget>
#include <QVBoxLayout>
#include <DStackedWidget>
#include "MainOperationWidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The LeftShowWidget class
 * @brief   侧边栏显示
 */


class LeftSidebarWidget : public DWidget
{
public:
    LeftSidebarWidget(DWidget *parent = nullptr);

private:
    void initOperationWidget();

private:
    QVBoxLayout             *m_pVBoxLayout = nullptr;

    DStackedWidget           *m_pStackedWidget = nullptr;
    MainOperationWidget     *m_operationWidget = nullptr;
};

#endif // LEFTSHOWWIDGET_H

#ifndef MAINSHOWDATAWIDGET_H
#define MAINSHOWDATAWIDGET_H

#include <DWidget>
#include "LeftSidebarWidget.h"


DWIDGET_USE_NAMESPACE

/**
 * @brief The MainShowDataWidget class
 * @brief   主要用于显示文档 和 操作界面
 */


class MainShowDataWidget : public DWidget
{
    Q_OBJECT
public:
    MainShowDataWidget(DWidget *parent = nullptr);

private:
    void initWidgets();

private slots:
    void setSidebarVisible(const bool&) const;

private:
    QVBoxLayout *m_mainLayout = nullptr;

    LeftSidebarWidget  *m_pLeftShowWidget = nullptr;
};

#endif // MAINSHOWDATAWIDGET_H

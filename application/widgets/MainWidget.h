#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <DWidget>
#include <QVBoxLayout>
#include <DStackedWidget>
#include "tabbar.h"

#include "MainShowDataWidget.h"
#include "HomeWidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainWidget class
 * @brief   主窗体显示
 */


class MainWidget : public DWidget
{
    Q_OBJECT
public:
    MainWidget(DWidget *parent = nullptr);

signals:
    void setShowSliderState(const bool&) const;
    void sigHomeOpenFile();

private slots:
    void showFileSelected(const QStringList) const;
    void setSliderState(const bool&) const;

private:
    void initWidgets();

private:    
    QVBoxLayout *m_centralLayout = nullptr;

    DStackedWidget      *m_pStackedWidget = nullptr;        //  栈式 显示

    HomeWidget          *m_pHomeWidget = nullptr;       //  选择文件
    MainShowDataWidget  *m_pMainShowDataWidget = nullptr;    //  文件显示 和 操作
};

#endif // MAINWIDGET_H

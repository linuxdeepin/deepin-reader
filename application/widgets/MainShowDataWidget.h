#ifndef MAINSHOWDATAWIDGET_H
#define MAINSHOWDATAWIDGET_H

#include <DWidget>
#include "LeftSidebarWidget.h"

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainShowDataWidget class
 * @brief   主要用于显示文档 和 操作界面
 */


class MainShowDataWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT
public:
    MainShowDataWidget(DWidget *parent = nullptr);
    ~MainShowDataWidget();

private:
    void initWidgets();

private slots:
    void setSidebarVisible(const bool&) const;
    void setFileHandShapeState(const bool&) const;
    void setFileMagnifyingState(const bool&) const;

private:
    QVBoxLayout *m_mainLayout = nullptr;

    LeftSidebarWidget   *m_pLeftShowWidget = nullptr;
    ThemeSubject        *m_pThemeSubject = nullptr;

    // IObserver interface
public:
    int update(const QString &);
};

#endif // MAINSHOWDATAWIDGET_H

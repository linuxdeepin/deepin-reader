#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <DWidget>
#include <QVBoxLayout>
#include <DStackedWidget>

#include "MainShowDataWidget.h"
#include "HomeWidget.h"
#include "FileAttrWidget.h"

#include "header/IThemeObserver.h"
#include "header/ThemeSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainWidget class
 * @brief   主窗体显示
 */


class MainWidget : public DWidget, public IThemeObserver
{
    Q_OBJECT
public:
    MainWidget(DWidget *parent = nullptr);
    ~MainWidget();

signals:
    void setShowSliderState(const bool&) const;     //  缩略图、书签、注释  侧边栏显隐 信号
    void setHandShapeState(const bool&) const;      //  手势 信号
    void setMagnifyingState(const bool&) const;     //  放大镜 信号

    void sigHomeOpenFile();

private slots:
    void showFileSelected(const QStringList) const;
    void slotOpenFileFolder();
    void slotFileAttr();

private:
    void initWidgets();

private:    
    QVBoxLayout *m_centralLayout = nullptr;

    DStackedWidget      *m_pStackedWidget = nullptr;        //  栈式 显示
    ThemeSubject        *m_pThemeSubject = nullptr;

    HomeWidget          *m_pHomeWidget = nullptr;       //  选择文件
    MainShowDataWidget  *m_pMainShowDataWidget = nullptr;    //  文件显示 和 操作

    FileAttrWidget      *m_pAttrWidget = nullptr;

    // IObserver interface
public:
    int update(const QString &);
};

#endif // MAINWIDGET_H

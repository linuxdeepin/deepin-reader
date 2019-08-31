#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <DWidget>
#include <QVBoxLayout>
#include <DStackedWidget>

#include "MainShowSplitter.h"
#include "HomeWidget.h"
#include "FileAttrWidget.h"

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainWidget class
 * @brief   主窗体显示
 */


class MainWidget : public DWidget, public IMsgObserver
{
    Q_OBJECT
public:
    MainWidget(DWidget *parent = nullptr);
    ~MainWidget() override;

private slots:
    void showFileSelected(const QStringList) const;
    void slotOpenFileFolder();
    void slotFileAttr();

private:
    void initWidgets();

private:    
    QVBoxLayout *m_centralLayout = nullptr;

    DStackedWidget      *m_pStackedWidget = nullptr;        //  栈式 显示
    MsgSubject        *m_pMsgSubject = nullptr;

    HomeWidget          *m_pHomeWidget = nullptr;       //  选择文件
    MainShowSplitter    *m_pMainShowSplitter = nullptr;    //  文件显示 和 操作

    FileAttrWidget      *m_pAttrWidget = nullptr;

    // IObserver interface
public:
    int update(const int&, const QString &) override;
};

#endif // MAINWIDGET_H

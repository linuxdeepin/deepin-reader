#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <DStackedWidget>
#include <QVBoxLayout>

#include "view/FileAttrWidget.h"
#include "HomeWidget.h"
#include "MainShowSplitter.h"
#include "FindBar.h"
#include "header/CustomWidget.h"
#include <DAnchors>

/**
 * @brief The MainWidget class
 * @brief   主窗体显示
 */

class MainWidget : public CustomWidget
{
    Q_OBJECT
public:
    MainWidget(CustomWidget *parent = nullptr);
    ~MainWidget() override;

private slots:
    void showFileSelected(const QStringList);

private:
    void openFileFolder();
    void showFileAttr();

private:
    QVBoxLayout *m_centralLayout = nullptr;

    DStackedWidget *m_pStackedWidget = nullptr; //  栈式 显示
    HomeWidget *m_pHomeWidget = nullptr; //  选择文件
    MainShowSplitter *m_pMainShowSplitter = nullptr; //  文件显示 和 操作
    FileAttrWidget *m_pAttrWidget = nullptr;
    FindBar *m_pFindBar = nullptr;

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // MAINWIDGET_H

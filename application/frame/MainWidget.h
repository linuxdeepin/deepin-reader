#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "subjectObserver/CustomWidget.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The MainWidget class
 * @brief   采用　栈式　显示窗口，　当前只显示某一特定窗口
 */


class MainWidget : public CustomWidget
{
    Q_OBJECT
public:
    MainWidget(CustomWidget *parent = nullptr);

private:
    void openFileFail(const QString &);
    void openFileOk();

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;
};

#endif // MAINSTACKWIDGET_H

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "CustomControl/CustomWidget.h"
#include <DSpinner>

/**
 * @brief The MainWidget class
 * @brief   采用　栈式　显示窗口，　当前只显示某一特定窗口
 */

class MainWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MainWidget)

public:
    explicit MainWidget(CustomWidget *parent = nullptr);

signals:
    void sigOpenFileOk();
    void sigDealWithData(const int &, const QString &);

protected:
    void dragEnterEvent(QDragEnterEvent *event) Q_DECL_OVERRIDE;
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;

private slots:
    void slotOpenFileOk();
    void slotDealWithData(const int &, const QString &msgContent = "");

private:
    void initConnections();

    void onOpenFileStart();
    void onOpenFileFail(const QString &);
    void onShowTips(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;
};

#endif  // MAINSTACKWIDGET_H

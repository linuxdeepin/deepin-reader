#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DPushButton>
#include <DToolButton>

#include "CustomControl/CustomWidget.h"

/**
 * @brief The MainOperationWidget class
 * @brief   缩略图、书签、注释 操作widget
 */

class MainOperationWidget : public CustomWidget
{
    Q_OBJECT
    Q_DISABLE_COPY(MainOperationWidget)

public:
    explicit MainOperationWidget(CustomWidget *parent = nullptr);
    ~MainOperationWidget() override;

signals:
    void sigShowStackWidget(const int &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
    int qDealWithData(const int &, const QString &) override;

    // CustomWidget interface
protected:
    void initWidget() override;

private:
    DToolButton *createBtn(const QString &btnName, const QString &objName);
    QString findBtnName();
    void initConnect();
    DPushButton *__CreateHideBtn();
    void __SetBtnCheckById(const int &);
    void __SearchExit();

    void onDocProxyMsg(const QString &);

private slots:
    void SlotDealWithData(const int &, const QString &);
    void slotOpenFileOk(const QString &sPath);
    void slotUpdateTheme();
    void slotButtonClicked(int);

};

#endif // MAINOPERATIONWIDGET_H

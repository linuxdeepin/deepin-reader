#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DIconButton>
#include <DPushButton>
#include <DToolButton>
#include "CustomControl/CustomWidget.h"
#include "controller/DataManager.h"

#include "controller/DBOperation/Abstraction.h"
#include "controller/DBOperation/AbstractImplement.h"

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
    ~MainOperationWidget() Q_DECL_OVERRIDE;

//    void setOperatAction(const int &index);

private:
    DToolButton *createBtn(const QString &btnName, const QString &objName);
    QString findBtnName();
    void initConnect();

signals:
    void sigDealWithData(const int &, const QString &);
//    void sigSearchControl();
//    void sigSearchClosed();
    void sigShowStackWidget(const int &);

private slots:
    void SlotDealWithData(const int &, const QString &);
    void slotUpdateTheme();
    void slotButtonClicked(int);
//    void slotSearchControl();

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

private:
    DPushButton *__CreateHideBtn();
    void __SetBtnCheckById(const int &);
    void __SearchExit();

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;

private:
    RefinedAbstractionA *m_pRefinedAbstractionA = nullptr;
};

#endif // MAINOPERATIONWIDGET_H

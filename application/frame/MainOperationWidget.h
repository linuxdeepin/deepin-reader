#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DIconButton>

#include "subjectObserver/CustomWidget.h"

/**
 * @brief The MainOperationWidget class
 * @brief   缩略图、书签、注释 操作widget
 */

class MainOperationWidget : public CustomWidget
{
    Q_OBJECT
public:
    MainOperationWidget(CustomWidget *parent = nullptr);

private:
    DIconButton *createBtn(const QString &btnName);

private slots:
    void slotButtonClicked(int);

    // CustomWidget interface
protected:
    void initWidget() Q_DECL_OVERRIDE;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
};

#endif // MAINOPERATIONWIDGET_H

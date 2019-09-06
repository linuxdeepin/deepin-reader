#ifndef MAINOPERATIONWIDGET_H
#define MAINOPERATIONWIDGET_H

#include <DImageButton>
#include <QHBoxLayout>

#include <QButtonGroup>
#include <DPushButton>

#include "PublicFunction.h"
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
    DPushButton *createBtn(const QString &btnName);

private slots:
    void slotButtonClicked(int);

    // CustomWidget interface
protected:
    void initWidget() override;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) override;
};

#endif // MAINOPERATIONWIDGET_H

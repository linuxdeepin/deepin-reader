#ifndef LIGHTEDWIDGET_H
#define LIGHTEDWIDGET_H

#include "header/CustomWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DLabel>
#include <DPushButton>


/**
 * @brief The LightedWidget class
 * @    高亮显示 widget
 */



class LightedWidget : public CustomWidget
{
    Q_OBJECT
public:
    LightedWidget(CustomWidget *parent = nullptr);

private:
    void createBtn(const QString &, const char *, QHBoxLayout *bottomLayout);

private slots:
    void SlotBtnRedClicked();
    void SlotBtnGreenClicked();
    void SlotBtnBlueClicked();

    // IObserver interface
public:
    int dealWithData(const int &, const QString &);

    // CustomWidget interface
protected:
    void initWidget();
};

#endif // LIGHTEDWIDGET_H

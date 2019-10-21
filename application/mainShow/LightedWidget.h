#ifndef LIGHTEDWIDGET_H
#define LIGHTEDWIDGET_H

#include "subjectObserver/CustomWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <DLabel>
#include <DFloatingButton>
#include <QPalette>

class LightedWidget : public CustomWidget
{
    Q_OBJECT
public:
    LightedWidget(CustomWidget *parent = nullptr);

private slots:
    void SlotOnBtnGroupClicked(int);

private:
    int     m_nOldId = -1;

    // IObserver interface
public:
    int dealWithData(const int &, const QString &);

    // CustomWidget interface
protected:
    void initWidget();
};

#endif // LIGHTEDWIDGET_H

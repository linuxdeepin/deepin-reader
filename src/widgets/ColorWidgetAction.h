#ifndef COLORWIDGETACTION_H
#define COLORWIDGETACTION_H

#include <DWidget>
#include <QWidgetAction>
#include "widgets/CustomClickLabel.h"

DWIDGET_USE_NAMESPACE

class ColorWidgetAction : public QWidgetAction
{
    Q_OBJECT
    Q_DISABLE_COPY(ColorWidgetAction)

public:
    explicit ColorWidgetAction(DWidget *pParent = nullptr);

public:
    int getIndex();

signals:
    void sigBtnGroupClicked();

private slots:
    void slotBtnClicked(int);
    void slotBtnDefaultClicked();

private:
    void initWidget(DWidget *pParent);
};

#endif  // COLORWIDGETACTION_H

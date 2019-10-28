#ifndef COLORWIDGETACTION_H
#define COLORWIDGETACTION_H

#include <QWidgetAction>
#include <DWidget>

DWIDGET_USE_NAMESPACE

class ColorWidgetAction : public QWidgetAction
{
    Q_OBJECT
public:
    ColorWidgetAction(DWidget *pParent = nullptr);

signals:
    void sigBtnGroupClicked(int);

private:
    void initWidget(DWidget *pParent);
};

#endif // COLORWIDGETACTION_H

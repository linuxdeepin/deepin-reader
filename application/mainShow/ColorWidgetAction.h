#ifndef COLORWIDGETACTION_H
#define COLORWIDGETACTION_H

#include <QWidgetAction>
#include <DWidget>
#include "CustomControl/CustomClickLabel.h"

DWIDGET_USE_NAMESPACE

class ColorWidgetAction : public QWidgetAction
{
    Q_OBJECT
    Q_DISABLE_COPY(ColorWidgetAction)

public:
    explicit ColorWidgetAction(DWidget *pParent = nullptr);

public:
    void setBtnAddLightState(const bool &);

signals:
    void sigBtnGroupClicked(int);
    void sigBtnDefaultClicked();

private:
    void initWidget(DWidget *pParent);

private:
    CustomClickLabel *m_pClickLabel = nullptr;
};

#endif // COLORWIDGETACTION_H

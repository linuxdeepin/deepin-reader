#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <DWidget>

#include "header/IMsgObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

class CustomWidget : public DWidget, public IMsgObserver
{
public:
    CustomWidget(DWidget *parent = nullptr);
    ~CustomWidget();

public:
    virtual int update(const int &, const QString &) = 0;

protected:
    MsgSubject  *m_pMsgSubject = nullptr;
};

#endif // CUSTOMWIDGET_H

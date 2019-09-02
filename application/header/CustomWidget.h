#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <DWidget>

#include "header/IObserver.h"
#include "header/MsgSubject.h"

DWIDGET_USE_NAMESPACE

/**
 * @brief The CustomWidget class
 * @brief   封装 DWidget 和 观察者， 便于 继承 DWidget
 *          实现 相应的消息处理， 更关注于 对应的业务处理
 */


class CustomWidget : public DWidget, public IObserver
{
    Q_OBJECT
public:
    CustomWidget(DWidget *parent = nullptr);
    ~CustomWidget();

protected:
    virtual void initWidget() = 0;

protected:
    MsgSubject  *m_pMsgSubject = nullptr;
};

#endif // CUSTOMWIDGET_H

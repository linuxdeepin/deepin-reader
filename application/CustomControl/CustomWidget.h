#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <DWidget>
#include <DGuiApplicationHelper>
#include <DPalette>

#include "subjectObserver/IObserver.h"
#include "controller/MsgSubject.h"
#include "controller/NotifySubject.h"
#include "subjectObserver/MsgHeader.h"
#include "subjectObserver/ModuleHeader.h"
#include "utils/utils.h"

#include <QDebug>

DWIDGET_USE_NAMESPACE


/**
 * @brief The CustomWidget class
 * @brief   封装 DWidget 和 观察者， 便于 继承 DWidget
 *          实现 相应的消息处理， 更关注于 对应的业务处理
 */

const int FIRST_LOAD_PAGES = 20;

class CustomWidget : public DWidget, public IObserver
{
    Q_OBJECT
public:
    CustomWidget(const QString &, DWidget *parent = nullptr);
    ~CustomWidget() Q_DECL_OVERRIDE;

    //  主题更新信号
signals:
    void sigUpdateTheme();

protected:
    virtual void initWidget() = 0;
    void updateWidgetTheme();

protected:
    void sendMsg(const int &msgType, const QString &msgContent = "") Q_DECL_OVERRIDE;
    void showScreenCenter();

private:
    MsgSubject      *m_pMsgSubject = nullptr;
    NotifySubject   *m_pNotifySubject = nullptr;
};

#endif // CUSTOMWIDGET_H

#ifndef CUSTOMWIDGET_H
#define CUSTOMWIDGET_H

#include <DWidget>

#include "IObserver.h"
#include "controller/MsgSubject.h"
#include "MsgHeader.h"
#include "ImageHeader.h"
#include "utils/utils.h"
#include "controller/viewerthememanager.h"

#include <QDebug>

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
    CustomWidget(const QString &, DWidget *parent = nullptr);
    ~CustomWidget() override;

protected:
    virtual void initWidget() = 0;

protected:
    void sendMsg(const int &msgType, const QString &msgContent = "") override;

private:
    void setObserverName(const QString &name) override;

private slots:
    void slotViewerThemeChanged(AppTheme);

private:
    MsgSubject  *m_pMsgSubject = nullptr;
};

#endif // CUSTOMWIDGET_H

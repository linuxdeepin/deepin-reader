#ifndef NOTIFYSUBJECT_H
#define NOTIFYSUBJECT_H

#include "ISubject.h"
#include "Singleton.h"

/**
 * @brief The NotifySubject class
 * @brief 消息推送服务
 */

class NotifySubject : public ISubject
{
    // ISubject interface
public:
    void addObserver(IObserver *obs) override;
    void removeObserver(IObserver *obs) override;

    // ISubject interface
public:
    void notifyMsg(const int &, const QString &msgContent = "");

private:
    void NotifyObservers(const int &, const QString &);

private:
    QList<IObserver *>  m_observerList;
};

typedef CSingleton<NotifySubject> g_NotifySubject;


#endif // NOTIFYSUBJECT_H

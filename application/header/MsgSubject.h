#ifndef MSGSUBJECT_H
#define MSGSUBJECT_H

#include "ISubject.h"
#include <QList>

/**
 * @brief The MsgSubject class
 * @brief   消息服务的 发布者
 */

class MsgSubject : public ISubject
{
private:
    MsgSubject();

public:
    static MsgSubject *getInstance()
    {
        static MsgSubject subject;
        return &subject;
    }

    // ISubject interface
public:
    void addObserver(IObserver *obs) override;
    void removeObserver(IObserver *obs) override;

public:
    void sendMsg(const int &, const QString &msgContent);

private:
    void NotifyObservers(const int &, const QString &);

private:
    QList<IObserver *> m_observerList;
};

#endif // MSGSUBJECT_H

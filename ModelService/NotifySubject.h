#ifndef NOTIFYSUBJECT_H
#define NOTIFYSUBJECT_H

#include <QMutex>

#include "SubjectThread.h"

/**
 * @brief The NotifySubject class
 * @brief 消息推送服务
 */
//  消息数据结构体
typedef struct {
    int msgType = -1;
    QString msgContent = "";
} MsgStruct;


class NotifySubject : public SubjectThread
{
private:
    NotifySubject();

public:
    static NotifySubject *Instance()
    {
        static NotifySubject ns;
        return &ns;
    }

    // ISubject interface
public:
    void addObserver(IObserver *obs) override;
    void removeObserver(IObserver *obs) override;

    // SubjectThread interface
public:
    void startThreadRun() override;
    void stopThreadRun() override;

    void notifyMsg(const int &, const QString &msgContent = "") override;

    // QThread interface
protected:
    void run() override;

private:
    void NotifyObservers(const int &, const QString &);

private:
    QList<IObserver *>  m_observerList;
    QList<MsgStruct>    m_msgList;
    bool                m_bRunFlag = false;

    QMutex              m_mutex;
    QMutex              m_obsMutex;

};


#endif // NOTIFYSUBJECT_H

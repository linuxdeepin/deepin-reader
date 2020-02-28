#ifndef NOTIFYSUBJECT_H
#define NOTIFYSUBJECT_H

#include <QMutex>

#include "SubjectThread.h"
#include "Singleton.h"
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
public:
    ~NotifySubject() override;
    // ISubject interface
public:
    void addObserver(IObserver *obs) override;
    void removeObserver(IObserver *obs) override;

    // QThread interface
protected:
    void run() override;

    // SubjectThread interface
public:
    void startThreadRun() override;
    void stopThreadRun() override;
    void notifyMsg(const int &, const QString &msgContent = "") override;

private:
    void NotifyObservers(const int &, const QString &);

private:
    QList<IObserver *>  m_observerList;
    QList<MsgStruct>    m_msgList;
    bool                m_bRunFlag = false;

    QMutex              m_mutex;
    QMutex              m_obsMutex;
};

typedef CSingleton<NotifySubject> g_NotifySubject;

#endif // NOTIFYSUBJECT_H

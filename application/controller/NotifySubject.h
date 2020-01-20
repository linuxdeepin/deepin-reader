#ifndef NOTIFYSUBJECT_H
#define NOTIFYSUBJECT_H

#include <QMutex>

#include "SubjectThread.h"
#include "subjectObserver/Singleton.h"

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
    // ISubject interface
public:
    void addObserver(IObserver *obs) Q_DECL_OVERRIDE;
    void removeObserver(IObserver *obs) Q_DECL_OVERRIDE;

    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE;

    // SubjectThread interface
public:
    void startThreadRun() Q_DECL_OVERRIDE;
    void stopThreadRun() Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &msgContent = "") Q_DECL_OVERRIDE;

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

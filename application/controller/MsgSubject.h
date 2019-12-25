#ifndef MSGSUBJECT_H
#define MSGSUBJECT_H

#include "subjectObserver/ISubject.h"
#include "subjectObserver/Singleton.h"

#include <QList>
#include <QThread>
#include <QObject>
#include <QMutex>
#include "SubjectHeader.h"
/**
 * @brief The MsgSubject class
 * @brief   采用线程的方式, 进行　消息的发送服务, 并由特定的observer进行处理， 截断！！
 * @brief   消息服务的 发布者
 */

class MsgSubject : public QThread, public ISubject
{

    // ISubject interface
public:
    void addObserver(IObserver *obs) Q_DECL_OVERRIDE;
    void removeObserver(IObserver *obs) Q_DECL_OVERRIDE;

    // QThread interface
protected:
    void run() Q_DECL_OVERRIDE;

public:
    void sendMsg(const int &, const QString &msgContent);
    void startThreadRun();
    void stopThreadRun();

private:
    int NotifyObservers(const int &, const QString &);

private:
    QList<IObserver *> m_observerList;
    QList<MsgStruct> m_msgList;
    QMutex      m_mutex;
    QMutex      m_obMutex;

    bool    m_bRunFlag = false;
};

typedef CSingleton<MsgSubject> g_MsgSubject;

#endif // MSGSUBJECT_H

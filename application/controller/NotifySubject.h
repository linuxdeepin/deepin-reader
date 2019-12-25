#ifndef NOTIFYSUBJECT_H
#define NOTIFYSUBJECT_H

#include "subjectObserver/ISubject.h"
#include "subjectObserver/Singleton.h"
#include <QThread>
#include "SubjectHeader.h"
#include <QMutex>

/**
 * @brief The NotifySubject class
 * @brief 消息推送服务
 */


class NotifySubject : public QThread, public ISubject
{
    Q_OBJECT
    Q_DISABLE_COPY(NotifySubject)

public:
    explicit NotifySubject(QObject *parent = nullptr);

    // ISubject interface
public:
    void addObserver(IObserver *obs);
    void removeObserver(IObserver *obs);

    // QThread interface
protected:
    void run();

public:
    void startThreadRun();
    void stopThreadRun();
    void notifyMsg(const int &, const QString &msgContent = "");

private:
    void NotifyObservers(const int &, const QString &);

private:
    QList<IObserver *>  m_observerList;
    QList<MsgStruct>    m_msgList;
    bool                m_bRunFlag = false;

    QMutex              m_mutex;
};

typedef CSingleton<NotifySubject> g_NotifySubject;

#endif // NOTIFYSUBJECT_H

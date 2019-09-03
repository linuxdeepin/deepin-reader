#ifndef MSGSUBJECT_H
#define MSGSUBJECT_H

#include "ISubject.h"
#include <QList>
#include <QThread>
#include <QObject>
#include <QMutex>

/**
 * @brief The MsgSubject class
 * @brief   采用线程的方式, 进行　消息的发送服务
 * @brief   消息服务的 发布者
 */

//  消息数据结构体
typedef struct {
    IObserver *obs;
    int msgType;
    QString msgContent;
} MsgStruct;

class MsgSubject : public QThread, public ISubject
{
    Q_OBJECT
private:
    MsgSubject(QObject *parent = nullptr);

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

    // QThread interface
protected:
    void run() override;

public:
    void sendMsg(IObserver *, const int &, const QString &msgContent);
    void stopThreadRun();

private:
    int NotifyObservers(const int &, const QString &);

private:
    QList<IObserver *> m_observerList;
    QList<MsgStruct> m_msgList;
    QMutex      m_mutex;

    bool    m_bRunFlag = false;
};

#endif // MSGSUBJECT_H

#ifndef MSGSUBJECT_H
#define MSGSUBJECT_H

#include "ISubject.h"
#include <QList>
#include <QThread>
#include <QObject>
#include <QMutex>

/**
 * @brief The MsgSubject class
 * @brief   采用线程的方式　进行消息的推送
 * @brief   消息服务的 发布者
 */

struct MsgStruct {
    IObserver *obs;
    int msgType;
    QString msgContent;
};

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

public:
    void sendMsg(IObserver *, const int &, const QString &msgContent);

private:
    int NotifyObservers(const int &, const QString &);

private:
    QList<IObserver *> m_observerList;

public:
    void setRunFlag(const bool &);
private:
    bool    m_bRunFlag = false;
    // QThread interface
protected:
    void run() override;

private:
    QList<MsgStruct> m_msgList;
    QMutex      m_mutex;
};

#endif // MSGSUBJECT_H

#include "NotifySubject.h"
#include "subjectObserver/MsgHeader.h"
#include <QDebug>

NotifySubject::NotifySubject(QObject *parent)
{
    Q_UNUSED(parent);
}

void NotifySubject::addObserver(IObserver *obs)
{
    m_observerList.append(obs);
}

void NotifySubject::removeObserver(IObserver *obs)
{
    m_observerList.removeOne(obs);
}

void NotifySubject::run()
{
    while (m_bRunFlag) {
        QList<MsgStruct> msgList;
        {
            QMutexLocker locker(&m_mutex);
            msgList = m_msgList;
            m_msgList.clear();
        }

        QListIterator<MsgStruct> iter(msgList);
        while (iter.hasNext()) {
            auto ms = iter.next();
            NotifyObservers(ms.msgType, ms.msgContent);
        }

        msleep(10);
    }
}

void NotifySubject::startThreadRun()
{
    m_bRunFlag = true;
    start();
}

void NotifySubject::stopThreadRun()
{
    m_bRunFlag = false;
    quit();
    wait();         //阻塞等待
}

void NotifySubject::notifyMsg(const int &msgType, const QString &msgContent)
{
    QMutexLocker locker(&m_mutex);

    MsgStruct msg;
    msg.msgType = msgType;
    msg.msgContent = msgContent;

    m_msgList.append(msg);

    qInfo() << "msgType = " << msgType << ",   msgContent = " << msgContent;
}

void NotifySubject::NotifyObservers(const int &msgType, const QString &msgContent)
{
    QListIterator<IObserver *> iter(m_observerList);
    while (iter.hasNext()) {
        auto obs = iter.next();
        if (obs) {
            int nRes = obs->dealWithData(msgType, msgContent);
            if (nRes == ConstantMsg::g_effective_res) {
                qInfo() << "msgType = " << msgType << ",   msgContent = " << msgContent;
                break;
            }
        }
    }
}

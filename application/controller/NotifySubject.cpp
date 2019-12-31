#include "NotifySubject.h"
#include "subjectObserver/MsgHeader.h"
#include <QDebug>

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

void NotifySubject::run()
{
    while (m_bRunFlag) {
        QList<MsgStruct> msgList;
        {
            QMutexLocker locker(&m_mutex);
            msgList = m_msgList;
            m_msgList.clear();
        }

        //  保证所有的消息 都被处理
        foreach (auto ms, msgList) {
            NotifyObservers(ms.msgType, ms.msgContent);
        }

        msleep(10);
    }
}

void NotifySubject::addObserver(IObserver *obs)
{
    QMutexLocker locker(&m_obsMutex);
    m_observerList.append(obs);
}

void NotifySubject::removeObserver(IObserver *obs)
{
    QMutexLocker locker(&m_obsMutex);
    m_observerList.removeOne(obs);
}

void NotifySubject::notifyMsg(const int &msgType, const QString &msgContent)
{
    QMutexLocker locker(&m_mutex);

    MsgStruct msg;
    msg.msgType = msgType;
    msg.msgContent = msgContent;

    m_msgList.append(msg);

#ifdef QT_DEBUG
    qInfo() << "debug   begin       msgType = " << msgType << ",   msgContent = " << msgContent;
#endif
}

void NotifySubject::NotifyObservers(const int &msgType, const QString &msgContent)
{
    QMutexLocker locker(&m_obsMutex);
    QListIterator<IObserver *> iter(m_observerList);
    while (iter.hasNext()) {
        auto obs = iter.next();
        if (obs) {
            int nRes = obs->dealWithData(msgType, msgContent);
            if (nRes == ConstantMsg::g_effective_res) {

#ifdef QT_DEBUG
                qInfo() << "msgType = " << msgType
                        << ",   msgContent = " << msgContent
                        << ",   deal = " << obs->getObserverName()
                        <<  "   end ";
#endif
                break;
            }
        }
    }
}

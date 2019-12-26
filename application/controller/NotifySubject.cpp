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

        foreach (auto ms, msgList) {
            NotifyObservers(ms.msgType, ms.msgContent);
        }

        msleep(10);
    }
}

void NotifySubject::addObserver(IObserver *obs)
{
    m_observerList.append(obs);
}

void NotifySubject::removeObserver(IObserver *obs)
{
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

int NotifySubject::NotifyObservers(const int &msgType, const QString &msgContent)
{
    int nRes = -1;
    QListIterator<IObserver *> iter(m_observerList);
    while (iter.hasNext()) {
        auto obs = iter.next();
        if (obs) {
            nRes = obs->dealWithData(msgType, msgContent);
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
    return nRes;
}

#include "NotifySubject.h"
#include "MsgHeader.h"
#include <QDebug>

//void NotifySubject::startThreadRun()
//{
//    m_bRunFlag = true;
//    start();
//}

//void NotifySubject::stopThreadRun()
//{
//    m_bRunFlag = false;
//    quit();
//    wait();         //阻塞等待
//}

//void NotifySubject::run()
//{
//    while (m_bRunFlag) {
//        QList<MsgStruct> msgList;
//        {
//            QMutexLocker locker(&m_mutex);
//            msgList = m_msgList;
//            m_msgList.clear();
//        }

//        //  保证所有的消息 都被处理
//        foreach (auto ms, msgList) {
//            NotifyObservers(ms.msgType, ms.msgContent);
//        }

//        msleep(10);
//    }
//}

NotifySubject::NotifySubject()
{
}

void NotifySubject::addObserver(IObserver *obs)
{
    m_observerList.append(obs);
}

void NotifySubject::removeObserver(IObserver *obs)
{
    m_observerList.removeOne(obs);
}

void NotifySubject::startThreadRun()
{
    m_bRunFlag = true;
    this->start();
}

void NotifySubject::stopThreadRun()
{
    m_bRunFlag = false;
    wait();
    quit();
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

        usleep(500);
    }
}
void NotifySubject::NotifyObservers(const int &msgType, const QString &msgContent)
{
    QListIterator<IObserver *> iter(m_observerList);
    while (iter.hasNext()) {
        auto obs = iter.next();
        if (obs != nullptr) {
            int nRes = obs->dealWithData(msgType, msgContent);
            if (nRes == ConstantMsg::g_effective_res) {
                break;
            }
        }
    }
}

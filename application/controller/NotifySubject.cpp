#include "NotifySubject.h"
#include "subjectObserver/MsgHeader.h"
#include <QDebug>

NotifySubject::NotifySubject(QObject *parent)
{
    Q_UNUSED(parent);

    //  默认启动线程，　只在　mainMainWindow 中　停止运行
    m_bRunFlag = true;
    start();
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

        if (msgList.size() > 0) {
            foreach (const MsgStruct &msg, msgList) {
                NotifyObservers(msg.msgType, msg.msgContent);
            }
        }
        msleep(50);
    }
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
    foreach (IObserver *obs, m_observerList) {
        int nRes = obs->dealWithData(msgType, msgContent);
        if (nRes == ConstantMsg::g_effective_res) {
            qInfo() << "msgType = " << msgType << ",   msgContent = " << msgContent;
            break;
        }
    }
}

#include "MsgSubject.h"
#include "subjectObserver/MsgHeader.h"

#include <QDebug>
#include <QMutexLocker>

MsgSubject::MsgSubject(QObject *parent)
{
    Q_UNUSED(parent);

    //  默认启动线程，　只在　mainMainWindow 中　停止运行
    m_bRunFlag = true;   
    start();    
}

void MsgSubject::addObserver(IObserver *obs)
{
    m_observerList.append(obs);
}

void MsgSubject::removeObserver(IObserver *obs)
{
    m_observerList.removeOne(obs);
}

void MsgSubject::sendMsg(IObserver *obs, const int &msgType, const QString &msgContent)
{
    QMutexLocker locker(&m_mutex);

    MsgStruct msg;
    msg.obs = obs;
    msg.msgType = msgType;
    msg.msgContent = msgContent;

    m_msgList.append(msg);

    QString sName = obs ? obs->getObserverName() : "MainWindow";

    qDebug() << "sendMsg   " << msgType
             <<  "  " << msgContent
             <<  "   " << sName;
}

int MsgSubject::NotifyObservers(const int &msgType, const QString &msgContent)
{
    /*
     * 如果 该消息 在某一个 观察者中被处理了， 就返回9999  则截断该消息，
     *  若没有处理， 则继续传递给下一个观察者，
     */
    foreach (IObserver *obs, m_observerList) {
        int nRes = obs->dealWithData(msgType, msgContent);
        if (nRes == ConstantMsg::g_effective_res) {

            QString sName = obs ? obs->getObserverName() : "MainWindow";

            qDebug() << "dealWithData   " << msgType
                     <<  "  " << msgContent
                     <<  "   " << sName;

            return 0;
        }
    }
    return -1;
}

void MsgSubject::stopThreadRun()
{
    m_bRunFlag = false;
    quit();
    wait();         //阻塞等待

}

void MsgSubject::run()
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
                int nRes = NotifyObservers(msg.msgType, msg.msgContent);
                if (nRes == 0) {
                    break;
                }
            }
        }
        msleep(50);
    }
}

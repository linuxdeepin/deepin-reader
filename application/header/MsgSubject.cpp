#include "MsgSubject.h"
#include "MsgHeader.h"

#include <QDebug>

MsgSubject::MsgSubject()
{

}

void MsgSubject::addObserver(IObserver *obs)
{
    m_observerList.append(obs);
}

void MsgSubject::removeObserver(IObserver *obs)
{
    m_observerList.removeOne(obs);
}

void MsgSubject::sendMsg(const int &msgType, const QString &msgContent)
{
    NotifyObservers(msgType, msgContent);
}

void MsgSubject::NotifyObservers(const int &msgType, const QString &msgContent)
{
    /*
     * 如果 该消息 在某一个 观察者中被处理了， 就返回9999  则截断该消息，
     *  若没有处理， 则继续传递给下一个观察者，
     */
    foreach (IObserver *obs, m_observerList) {
        int nRes = obs->dealWithData(msgType, msgContent);
        if (nRes == ConstantMsg::g_effective_res) {
            qDebug() << "dealWithData   " << msgType
                     <<  "  " << msgContent <<  "   " << obs->getObserverName();
            break;
        }
    }
}

#include "NotifySubject.h"
#include "MsgHeader.h"
#include <QDebug>


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
    QListIterator<IObserver *> iter(m_observerList);
    while (iter.hasNext()) {
        auto obs = iter.next();
        if (obs != nullptr) {
            int nRes = obs->dealWithData(msgType, msgContent);
            if (nRes == MSG_OK) {
                break;
            }
        }
    }
}

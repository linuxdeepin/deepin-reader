#ifndef IOBSERVER_H
#define IOBSERVER_H

#include <QString>

/**
 * @brief The IObserver class
 * @brief   订阅者 虚基类
 */

class IObserver
{
protected:
    virtual ~IObserver() {}

public:
    virtual int dealWithData(const int &, const QString &) = 0;
    virtual void sendMsg(const int &, const QString &) = 0;
    virtual void notifyMsg(const int &msgType, const QString &msgContent = "") = 0;

public:
    QString getObserverName()
    {
        return m_strObserverName;
    }

protected:
    QString     m_strObserverName = "";
};


#endif // IOBSERVER_H

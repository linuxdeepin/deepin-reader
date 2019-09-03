#ifndef IOBSERVER_H
#define IOBSERVER_H

#include <QString>

/**
 * @brief The IObserver class
 * @brief   订阅者 虚基类
 */

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"

class IObserver
{
protected:
    virtual ~IObserver() {}

public:
    virtual int dealWithData(const int &, const QString &) = 0;
    virtual void sendMsg(const int &, const QString &) = 0;

    //  添加订阅者身份，用于具体定位到某个订阅者
public:
    void setObserverName(const QString &name)
    {
        m_strObserverName = name;
    }
    QString getObserverName()
    {
        return m_strObserverName;
    }

private:
    QString     m_strObserverName = "";
};

#pragma clang diagnostic pop

#endif // IOBSERVER_H

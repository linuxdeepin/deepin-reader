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

    //  添加订阅者身份，用于具体定位到某个订阅者
private:
    virtual void setObserverName() = 0;

public:
    QString getObserverName()
    {
        return m_strObserverName;
    }

protected:
    QString     m_strObserverName = "";
};


#endif // IOBSERVER_H

#ifndef IOBSERVER_H
#define IOBSERVER_H

#include <QString>

/**
 * @brief The IObserver class
 * @brief   订阅者 虚基类
 */

class IObserver
{
public:
    virtual ~IObserver() {}

public:
    virtual int dealWithData(const int &, const QString &) {}

    virtual void notifyMsg(const int &, const QString &) {}

public:
    QString getObserverName()
    {
        return m_strObserverName;
    }

protected:
    QString     m_strObserverName = "";
};


#endif // IOBSERVER_H

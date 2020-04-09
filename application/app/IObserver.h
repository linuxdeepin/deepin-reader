#ifndef IOBSERVER_H
#define IOBSERVER_H

#include <QString>
#include "ModuleHeader.h"

class IObserver
{
public:
    virtual ~IObserver() {}

public:
    virtual int dealWithData(const int &, const QString &)
    {
        return MSG_OK;
    }

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

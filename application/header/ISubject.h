#ifndef ISUBJECT_H
#define ISUBJECT_H

#include "IObserver.h"
#include <QString>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"

/**
 * @brief The ISubject class
 * @brief   发布者 虚基类
 */


class ISubject
{
protected:
    virtual ~ISubject(){}

public:
    virtual void addObserver(IObserver* obs) = 0;
    virtual void removeObserver(IObserver* obs) = 0;
    virtual void Notify(const QString&) = 0;
};

#pragma clang diagnostic pop

#endif // ISUBJECT_H

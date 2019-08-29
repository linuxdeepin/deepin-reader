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
public:
    virtual int update(const QString&) = 0;

protected:
    virtual ~IObserver(){}
};


#pragma clang diagnostic pop

#endif // IOBSERVER_H

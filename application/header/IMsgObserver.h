#ifndef IMSGOBSERVER_H
#define IMSGOBSERVER_H

#include "IObserver.h"

class IMsgObserver : public IObserver
{
public:
    virtual int update(const int &, const QString &) = 0;
};

#endif // IMsgObserver_H

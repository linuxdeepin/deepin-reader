#ifndef ITHEMEOBSERVER_H
#define ITHEMEOBSERVER_H

#include "IObserver.h"

class IThemeObserver : public IObserver
{
public:
     virtual int update(const QString&) = 0;
};

#endif // ITHEMEOBSERVER_H

#ifndef SUBJECTTHREAD_H
#define SUBJECTTHREAD_H

#include <QThread>
#include "subjectObserver/ISubject.h"

/**
 * @brief The SubjectThread class
 *  主题线程通知类
 */

class SubjectThread : public QThread, public ISubject
{
protected:
    virtual void startThreadRun() = 0;
    virtual void stopThreadRun() = 0;
    virtual void notifyMsg(const int &, const QString &) = 0;
};

#endif // SUBJECTTHREAD_H

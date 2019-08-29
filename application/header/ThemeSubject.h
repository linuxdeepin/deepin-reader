#ifndef THEMESUBJECT_H
#define THEMESUBJECT_H

#include "ISubject.h"
#include <QList>

/**
 * @brief The ThemeSubject class
 * @brief   主题更新， 发布者服务
 */


class ThemeSubject : public ISubject
{
private:
    ThemeSubject();

public:
    static ThemeSubject* getInstace()
    {
        static ThemeSubject instance;
        return &instance;
    }

    // ISubject interface
public:
    void addObserver(IObserver *obs);
    void removeObserver(IObserver *obs);
    void Notify(const QString&);

private:
    QList<IObserver*> m_observerList;
};

#endif // THEMESUBJECT_H

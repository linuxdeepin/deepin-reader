#include "ThemeSubject.h"

ThemeSubject::ThemeSubject()
{

}

void ThemeSubject::addObserver(IObserver *obs)
{
    m_observerList.append(obs);
}

void ThemeSubject::removeObserver(IObserver *obs)
{
    m_observerList.removeOne(obs);
}

void ThemeSubject::Notify(const QString& inputData)
{
    foreach(IObserver * obs, m_observerList)
    {
        obs->update(inputData);
    }
}

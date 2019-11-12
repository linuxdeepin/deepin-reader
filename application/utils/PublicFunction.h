#ifndef PUBLICFUNCTION_H
#define PUBLICFUNCTION_H

#include <QString>
#include <QDebug>
#include "controller/DataManager.h"


namespace PF {
static QString getImagePath(const QString &imageName, const QString &priName)
{
    QString sTheme = DataManager::instance()->gettrCurrentTheme();
    QString strfilepath=QString(":/resources/%1/%2/%3.svg").arg(sTheme).arg(priName).arg(imageName);   
    return strfilepath;
}

}

#endif // PUBLICFUNCTION_H

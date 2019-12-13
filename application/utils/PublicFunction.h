#ifndef PUBLICFUNCTION_H
#define PUBLICFUNCTION_H

#include <QDebug>
#include <QIcon>
#include <QString>
#include "controller/DataManager.h"

namespace PF {

static QString getImagePath(const QString &imageName, const QString &priName)
{
    QString sTheme = DataManager::instance()->gettrCurrentTheme();
    //    QString
    //    strfilepath=QString(":/resources/%1/%2/%3.svg").arg(sTheme).arg(priName).arg(imageName);
    QString strfilepath =
        QString(":/icons/deepin/builtin/%1/%2/%3.svg").arg(sTheme).arg(priName).arg(imageName);
    return strfilepath;
}

static QIcon getIcon(const QString &iconName)
{
    return QIcon::fromTheme(iconName);
}

static QString getIconPath(const QString &iconName)
{
    QString strfilepath = QString(":/icons/deepin/builtin/%1.svg").arg(iconName);
    return strfilepath;
}

}  // namespace PF

#endif  // PUBLICFUNCTION_H

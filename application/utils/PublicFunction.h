#ifndef PUBLICFUNCTION_H
#define PUBLICFUNCTION_H

#include <QDebug>
#include <QIcon>
#include <QString>
#include <DGuiApplicationHelper>

#include "application.h"

#include "business/AppInfo.h"

namespace PF {

static QString getImagePath(const QString &imageName, const QString &priName)
{
    DGuiApplicationHelper::ColorType colorType = DGuiApplicationHelper::instance()->themeType();
    QString sTheme = "";
    if (colorType == DGuiApplicationHelper::UnknownType) {  //  未知
        sTheme = "Unknown";
    } else if (colorType == DGuiApplicationHelper::LightType) { //  浅色
        sTheme = "light";
    } else if (colorType == DGuiApplicationHelper::DarkType) {  //  深色
        sTheme = "dark";
    }

    QString strfilepath = QString(":/icons/deepin/builtin/%1/%2/%3.svg").arg(sTheme).arg(priName).arg(imageName);
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

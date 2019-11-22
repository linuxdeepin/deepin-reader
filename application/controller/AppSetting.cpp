#include "AppSetting.h"
#include "utils/utils.h"
#include <QDir>

AppSetting::AppSetting(QObject *parent)
    : QObject(parent)
{
    m_pSettings = new QSettings(QDir(Utils::getConfigPath()).filePath("config.conf"), QSettings::IniFormat, parent);
}

void AppSetting::setKeyValue(const QString &sKey, const QString &sValue)
{
    // initalize the configuration file.
    if (m_pSettings->value(sKey).toString().isEmpty()) {
        m_pSettings->setValue(sKey, sValue);
    }
}

QString AppSetting::getKeyValue(const QString &sKey) const
{
    return  m_pSettings->value(sKey).toString();
}

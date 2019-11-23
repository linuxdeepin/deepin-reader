#include "AppSetting.h"
#include "utils/utils.h"
#include <QDir>
#include "DataManager.h"

AppSetting::AppSetting(QObject *parent)
    : QObject(parent)
{
    m_pSettings = new QSettings(QDir(Utils::getConfigPath()).filePath("config.conf"), QSettings::IniFormat, parent);
}

void AppSetting::setKeyValue(const int &iKey, const QString &sValue)
{
    // initalize the configuration file.
    QString sKey = QString("%1").arg(iKey);

    QString ssFilePath = DataManager::instance()->strOnlyFilePath();
    QString ssValue = QString("%1@#&%2").arg(ssFilePath).arg(sValue);

    if (sValue == "") {
        m_pSettings->remove(sKey);
    } else {
        m_pSettings->setValue(sKey, ssValue);
    }
}

QString AppSetting::getKeyValue(const int &iKey) const
{
    QString sKey = QString("%1").arg(iKey);
    QString sTempValue = m_pSettings->value(sKey).toString();
    QStringList sTempList = sTempValue.split("@#&", QString::SkipEmptyParts);
    if (sTempList.size() == 2) {
        QString sFilePath = sTempList.at(0);
        QString ssFilePath = DataManager::instance()->strOnlyFilePath();

        if (sFilePath == ssFilePath) {
            return sTempList.at(1);
        }
    }

    return "";
}

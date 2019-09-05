#include "DataManager.h"

DataManager::DataManager(QObject *parent) :
    QObject (parent)
{
    m_strOnlyFilePath = "";
}

QString DataManager::strOnlyFilePath() const
{
    return m_strOnlyFilePath;
}

void DataManager::setStrOnlyFilePath(const QString &strOnlyFilePath)
{
    m_strOnlyFilePath = strOnlyFilePath;
}

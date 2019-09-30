#include "DataManager.h"
#include "application.h"

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
    m_bIsUpdate = false;
    dApp->dbM->setStrFilePath(strOnlyFilePath);

    m_strOnlyFilePath = strOnlyFilePath;
}

int DataManager::CurShowState() const
{
    return m_nCurShowState;
}

void DataManager::setCurShowState(int nCurShowState)
{
    m_nCurShowState = nCurShowState;
}

bool DataManager::bIsUpdate() const
{
    return m_bIsUpdate;
}

void DataManager::setBIsUpdate(bool bIsUpdate)
{
    m_bIsUpdate = bIsUpdate;
}

bool DataManager::bIsBookMarkState() const
{
    return m_bIsBookMarkState;
}

void DataManager::setBIsBookMarkState(bool bIsBookMarkState)
{
    m_bIsBookMarkState = bIsBookMarkState;
}

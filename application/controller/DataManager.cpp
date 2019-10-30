#include "DataManager.h"
#include "application.h"

DataManager::DataManager(QObject *parent) :
    QObject (parent)
{
    m_strOnlyFilePath = "";

    m_listColor.append((Qt::darkYellow));
    m_listColor.append((Qt::red));
    m_listColor.append((Qt::darkMagenta));
    m_listColor.append((Qt::blue));
    m_listColor.append((Qt::cyan));
    m_listColor.append((Qt::green));
    m_listColor.append((Qt::yellow));
    m_listColor.append((Qt::gray));
}

QString DataManager::strOnlyFilePath() const
{
    return m_strOnlyFilePath;
}

void DataManager::setStrOnlyFilePath(const QString &strOnlyFilePath)
{
    if (strOnlyFilePath == "") {
        m_bIsUpdate = true;
    } else {
        m_bIsUpdate = false;
    }
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

QList<QColor> DataManager::getLightColorList()
{
    return m_listColor;
}

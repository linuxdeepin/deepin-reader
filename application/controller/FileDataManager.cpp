#include "FileDataManager.h"

FileDataManager::FileDataManager(QObject *parent) :
    QObject(parent)
{

}

int FileDataManager::dealWithData(const int &, const QString &)
{
    return 0;
}

void FileDataManager::sendMsg(const int &, const QString &)
{

}

void FileDataManager::notifyMsg(const int &, const QString &)
{

}

QString FileDataManager::qGetCurrentFilePath() const
{
    return m_strCurrentFilePath;
}

void FileDataManager::qSetCurrentFilePath(const QString &strFilePath)
{
    m_strCurrentFilePath = strFilePath;
}

FileData FileDataManager::qGetFileData(const QString &sPath) const
{
    return m_pFileStateMap[sPath];
}

void FileDataManager::qSetFileData(const QString &sPath, const FileData &fd)
{
    m_pFileStateMap[sPath] = fd;
}

QMap<QString, FileData> FileDataManager::qGetFileStateMap() const
{
    return m_pFileStateMap;
}

QMap<QString, QString> FileDataManager::qGetFileAndUuidMap() const
{
    return m_pFileAndUuidMap;
}

void FileDataManager::qInsertFileAndUuid(const QString &sPath, const QString &sUuid)
{
    m_pFileAndUuidMap.insert(sPath, sUuid);
}

QString FileDataManager::qGetFileUuid(const QString &sPath)
{
    return  m_pFileAndUuidMap[sPath];
}

QList<QString> FileDataManager::qGetOpenFilePathList() const
{
    return m_pFileStateMap.keys();
}

void FileDataManager::qInsertOpenFilePath(const QString &strPath)
{
    m_pFileStateMap.insert(strPath, FileData());
}

void FileDataManager::qRemoveFilePath(const QString &strPath)
{
    m_pFileStateMap.remove(strPath);
    m_pFileAndUuidMap.remove(strPath);
}

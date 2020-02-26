#include "FileDataManager.h"
#include "MsgModel.h"

FileDataManager::FileDataManager(QObject *parent) :
    QObject(parent)
{
    InitConnection();

    dApp->m_pModelService->addObserver(this);
}

int FileDataManager::dealWithData(const int &msgType, const QString &msgContent)
{
    if (E_FILE_MSG == msgType) {
        emit sigFileChange(msgContent);
    }

    return 0;
}

void FileDataManager::sendMsg(const int &, const QString &)
{

}

void FileDataManager::notifyMsg(const int &, const QString &)
{

}

void FileDataManager::InitConnection()
{
    connect(this, SIGNAL(sigFileChange(const QString &)), SLOT(slotFileChange(const QString &)));
}

void FileDataManager::slotFileChange(const QString &sContent)
{
    MsgModel mm;
    mm.fromJson(sContent);

    int nType = mm.getMsgType();
    QString sPath = mm.getPath();
    QString sValue = mm.getValue();

    if (nType == MSG_FILE_IS_CHANGE) {
        setFileChange(sValue);
    } else if (nType == MSG_WIDGET_THUMBNAILS_VIEW) {
        setThumbnailState(sValue);
    }
}

void FileDataManager::setFileChange(const QString &sContent)
{
    qInsertFileChange(m_strCurrentFilePath, sContent.toInt());
}

void FileDataManager::setThumbnailState(const QString &sValue)
{
    FileDataModel fdm = qGetFileData(m_strCurrentFilePath);
    fdm.qSetData(Thumbnail, sValue.toInt());

    qSetFileData(m_strCurrentFilePath, fdm);
}

QString FileDataManager::qGetCurrentFilePath() const
{
    return m_strCurrentFilePath;
}

void FileDataManager::qSetCurrentFilePath(const QString &strFilePath)
{
    m_strCurrentFilePath = strFilePath;
}

FileDataModel FileDataManager::qGetFileData(const QString &sPath) const
{
    return m_pFileStateMap[sPath];
}

void FileDataManager::qSetFileData(const QString &sPath, const FileDataModel &fd)
{
    m_pFileStateMap[sPath] = fd;
}

QMap<QString, FileDataModel> FileDataManager::qGetFileStateMap() const
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

QMap<QString, FileState> FileDataManager::qGetFileChangeMap() const
{
    return m_pFileChangeMap;
}

void FileDataManager::qInsertFileChange(const QString &sPath, const int &nState)
{
    if (!m_pFileChangeMap.contains(sPath))
        m_pFileChangeMap[sPath].isChange = nState;
    else {
        FileState fs;
        fs.isChange = nState;
        m_pFileChangeMap.insert(sPath, fs);
    }
}

void FileDataManager::qInsertFileOpen(const QString &sPath, const int &iOpen)
{
    if (!m_pFileChangeMap.contains(sPath))
        m_pFileChangeMap[sPath].isOpen = iOpen;
    else {
        FileState fs;
        fs.isOpen = iOpen;
        m_pFileChangeMap.insert(sPath, fs);
    }
}

FileState FileDataManager::qGetFileChange(const QString &sPath)
{
    return m_pFileChangeMap[sPath];
}

QList<QString> FileDataManager::qGetOpenFilePathList() const
{
    return m_pFileStateMap.keys();
}

void FileDataManager::qInsertOpenFilePath(const QString &strPath)
{
    m_pFileStateMap.insert(strPath, FileDataModel());
}

void FileDataManager::qRemoveFilePath(const QString &strPath)
{
    m_pFileStateMap.remove(strPath);
    m_pFileAndUuidMap.remove(strPath);
}

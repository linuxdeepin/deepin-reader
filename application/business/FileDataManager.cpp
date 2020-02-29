#include "FileDataManager.h"
#include "MsgModel.h"

FileDataManager::FileDataManager(QObject *parent) :
    QObject(parent)
{
    m_strObserverName = "FileDataManager";
    InitConnection();

    dApp->m_pModelService->addObserver(this);
}

int FileDataManager::dealWithData(const int &msgType, const QString &msgContent)
{
    if (E_FILE_MSG == msgType) {
        emit sigFileChange(msgContent);
    } else if (E_TITLE_MSG_TYPE == msgType) {
        emit sigHistroyChange(msgContent);
    } else if (E_APP_MSG_TYPE == msgType) {
        emit sigAppExitNothing(msgContent);
    }

    return 0;
}

void FileDataManager::sendMsg(const int &, const QString &)
{

}

void FileDataManager::notifyMsg(const int &, const QString &)
{

}

QString FileDataManager::qGetCurUuid() const
{
    auto it = m_pFileAndUuidMap.find(m_strCurrentFilePath);
    return  it != m_pFileAndUuidMap.end() ? it.value() : nullptr;
}

void FileDataManager::InitConnection()
{
    connect(this, SIGNAL(sigFileChange(const QString &)), SLOT(slotFileChange(const QString &)));
    connect(this, SIGNAL(sigHistroyChange(const QString &)), SLOT(slotHistroyChange(const QString &)));
    connect(this, SIGNAL(sigAppExitNothing(const QString &)), SLOT(slotAppExitNothing(const QString &)));
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
    }
}

void FileDataManager::slotHistroyChange(const QString &sContent)
{
    MsgModel mm;
    mm.fromJson(sContent);

    int nType = mm.getMsgType();
    QString sValue = mm.getValue();

    if (nType == MSG_WIDGET_THUMBNAILS_VIEW) {
        setThumbnailState(sValue);
    } else if (nType == MSG_LEFTBAR_STATE) {
        SetLeftWidgetIndex(sValue);
    } else if (nType == MSG_VIEWCHANGE_DOUBLE_SHOW) {
        OnSetViewChange(sValue);
    } else if (nType == MSG_VIEWCHANGE_ROTATE) {
        OnSetViewRotate(sValue);
    } else if (nType == MSG_FILE_SCALE) {
        OnSetViewScale(sValue);
    } else if (nType == MSG_VIEWCHANGE_FIT) {
        OnSetViewHit(sValue);
    } else if (nType == MSG_FILE_PAGE_CHANGE) {

    }
}

void FileDataManager::slotAppExitNothing(const QString &sContent)
{
    MsgModel mm;
    mm.fromJson(sContent);

    int nMsg = mm.getMsgType();
    if (APP_EXIT_NOTHING == nMsg) {
        QList<QString> fileList = qGetOpenFilePathList();
        foreach (const QString &s, fileList) {
            qSaveData(s);
        }

        dApp->exit(0);
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

void FileDataManager::SetLeftWidgetIndex(const QString &sValue)
{
    FileDataModel fdm = qGetFileData(m_strCurrentFilePath);
    fdm.qSetData(LeftIndex, sValue.toInt());

    qSetFileData(m_strCurrentFilePath, fdm);
}

void FileDataManager::OnSetViewChange(const QString &sValue)
{
    FileDataModel fdm = qGetFileData(m_strCurrentFilePath);
    fdm.qSetData(DoubleShow, sValue.toInt());

    qSetFileData(m_strCurrentFilePath, fdm);
}

void FileDataManager::OnSetViewScale(const QString &sValue)
{
    FileDataModel fdm = qGetFileData(m_strCurrentFilePath);
    fdm.qSetData(Scale, sValue.toInt());

    qSetFileData(m_strCurrentFilePath, fdm);
}

void FileDataManager::OnSetViewRotate(const QString &sValue)
{
    FileDataModel fdm = qGetFileData(m_strCurrentFilePath);
    fdm.qSetData(Rotate, sValue.toInt());

    qSetFileData(m_strCurrentFilePath, fdm);
}

void FileDataManager::OnSetViewHit(const QString &sValue)
{
    FileDataModel fdm = qGetFileData(m_strCurrentFilePath);
    fdm.qSetData(Fit, sValue.toInt());

    qSetFileData(m_strCurrentFilePath, fdm);
}

void FileDataManager::OnSetCurPage(const QString &sValue)
{
    FileDataModel fdm = qGetFileData(m_strCurrentFilePath);
    fdm.qSetData(CurPage, sValue.toInt());

    qSetFileData(m_strCurrentFilePath, fdm);
}

QString FileDataManager::qGetCurrentFilePath() const
{
    return m_strCurrentFilePath;
}

void FileDataManager::qSetCurrentFilePath(const QString &strFilePath)
{
    if (strFilePath == "")
        return;

    m_strCurrentFilePath = strFilePath;
}

FileDataModel FileDataManager::qGetFileData(const QString &sPath) const
{
    if (sPath == "")
        return FileDataModel();

    return m_pFileStateMap[sPath];
}

void FileDataManager::qSetFileData(const QString &sPath, const FileDataModel &fd)
{
    if (sPath == "")
        return;

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
    if (sPath == "")
        return;

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
    if (sPath == "")
        return;

    if (m_pFileChangeMap.contains(sPath))
        m_pFileChangeMap[sPath].isChange = nState;
    else {
        FileState fs;
        fs.isChange = nState;
        m_pFileChangeMap.insert(sPath, fs);
    }
}

void FileDataManager::qInsertFileOpen(const QString &sPath, const int &iOpen)
{
    if (sPath == "")
        return;

    if (m_pFileChangeMap.contains(sPath))
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

void FileDataManager::qSaveData(const QString &sPath)
{
    if (sPath == "")
        return;

    FileDataModel fdm = dApp->m_pDataManager->qGetFileData(sPath);

    for (int iLoop = Scale; iLoop < CurPage + 1; iLoop++) {
        dApp->m_pDBService->setHistroyData(sPath, iLoop, fdm.qGetData(iLoop));
    }
    dApp->m_pDBService->qSaveData(sPath, DB_HISTROY);
}

DocummentProxy *FileDataManager::qGetCurrentProxy()
{
    QString sCurPath = dApp->m_pDataManager->qGetCurrentFilePath();
    if (sCurPath != "") {
        QString sUuid = dApp->m_pDataManager->qGetFileUuid(sCurPath);
        if (sUuid != "") {
            DocummentProxy *_proxy = DocummentProxy::instance(sUuid);
            if (_proxy) {
                return _proxy;
            }
        }
    }
    return nullptr;
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

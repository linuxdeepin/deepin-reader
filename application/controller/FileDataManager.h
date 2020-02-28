#ifndef FILEDATAMANAGER_H
#define FILEDATAMANAGER_H

#include <QObject>
#include <QMap>

#include "application.h"
#include "FileDataModel.h"
#include "FileStateHeader.h"

#include "docview/docummentproxy.h"

/**
 * @brief The DataManager class
 * @brief   固定数据管理区， 采用 单例
 */

class FileDataManager : public QObject, public IObserver
{
    Q_OBJECT
    Q_DISABLE_COPY(FileDataManager)

public:
    explicit FileDataManager(QObject *parent = nullptr);

signals:
    void sigFileChange(const QString &);
    void sigHistroyChange(const QString &);
    void sigAppExitNothing(const QString &);

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &) Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &) Q_DECL_OVERRIDE;

public:
    QString qGetCurUuid()const;
    QString qGetCurrentFilePath() const;
    void qSetCurrentFilePath(const QString &strFilePath);

    QList<QString> qGetOpenFilePathList() const;
    void qInsertOpenFilePath(const QString &strPath);
    void qRemoveFilePath(const QString &strPath);

    FileDataModel qGetFileData(const QString &) const;
    void qSetFileData(const QString &strPath, const FileDataModel &);
    QMap<QString, FileDataModel> qGetFileStateMap() const;

    QMap<QString, QString> qGetFileAndUuidMap() const;
    void qInsertFileAndUuid(const QString &, const QString &);
    QString qGetFileUuid(const QString &);

    QMap<QString, FileState> qGetFileChangeMap() const;
    void qInsertFileChange(const QString &, const int &);
    void qInsertFileOpen(const QString &, const int &);
    FileState qGetFileChange(const QString &);

    void qSaveData(const QString &sPath);
    DocummentProxy  *qGetCurrentProxy();

private:
    void InitConnection();

    void setFileChange(const QString &);
    void setThumbnailState(const QString &);
    void SetLeftWidgetIndex(const QString &sValue);
    void OnSetViewChange(const QString &);
    void OnSetViewScale(const QString &);
    void OnSetViewRotate(const QString &);
    void OnSetViewHit(const QString &);

private slots:
    void slotFileChange(const QString &);
    void slotHistroyChange(const QString &);
    void slotAppExitNothing(const QString &);

private:
    QString m_strCurrentFilePath = "";              //  当前显示的文档路径
    QMap<QString, FileDataModel>    m_pFileStateMap;        //  已打开的文档列表
    QMap<QString, QString>          m_pFileAndUuidMap;       //  已打开的文档列表
    QMap<QString, FileState>        m_pFileChangeMap;
};

#endif // DATAMANAGER_H

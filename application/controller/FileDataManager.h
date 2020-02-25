#ifndef FILEDATAMANAGER_H
#define FILEDATAMANAGER_H

#include <QObject>
#include <QMap>

#include "application.h"
#include "FileData.h"

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

    // IObserver interface
public:
    int dealWithData(const int &, const QString &) Q_DECL_OVERRIDE;
    void sendMsg(const int &, const QString &) Q_DECL_OVERRIDE;
    void notifyMsg(const int &, const QString &) Q_DECL_OVERRIDE;

public:
    QString qGetCurrentFilePath() const;
    void qSetCurrentFilePath(const QString &strFilePath);

    QList<QString> qGetOpenFilePathList() const;
    void qInsertOpenFilePath(const QString &strPath);
    void qRemoveFilePath(const QString &strPath);

    FileData qGetFileData(const QString &) const;
    void qSetFileData(const QString &strPath, const FileData &);
    QMap<QString, FileData> qGetFileStateMap() const;

    QMap<QString, QString> qGetFileAndUuidMap() const;
    void qInsertFileAndUuid(const QString &, const QString &);
    QString qGetFileUuid(const QString &);

private:
    QString m_strCurrentFilePath = "";              //  当前显示的文档路径
    QMap<QString, FileData> m_pFileStateMap;        //  已打开的文档列表
    QMap<QString, QString> m_pFileAndUuidMap;       //  已打开的文档列表
};

#endif // DATAMANAGER_H

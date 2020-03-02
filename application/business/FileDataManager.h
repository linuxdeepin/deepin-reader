#ifndef FILEDATAMANAGER_H
#define FILEDATAMANAGER_H

#include <QObject>
#include <QMap>

#include "application.h"
#include "FileDataModel.h"

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
    int dealWithData(const int &, const QString &) override;
    void sendMsg(const int &, const QString &) override;
    void notifyMsg(const int &, const QString &) override;

public:
    QString qGetCurrentFilePath() const;
    void qSetCurrentFilePath(const QString &strFilePath);

    QList<QString> qGetOpenFilePathList() const;
    void qInsertOpenFilePath(const QString &strPath);
    void qRemoveFilePath(const QString &strPath);

    FileDataModel qGetFileData(const QString &) const;
    void qSetFileData(const QString &strPath, const FileDataModel &);
    QMap<QString, FileDataModel> qGetFileStateMap() const;

    void qSaveData(const QString &sPath);

private:
    void InitConnection();

//    void setFileChange(const QString &);
    void setThumbnailState(const QString &);
    void SetLeftWidgetIndex(const QString &sValue);
    void OnSetViewChange(const QString &);
    void OnSetViewScale(const QString &);
    void OnSetViewRotate(const QString &);
    void OnSetViewHit(const QString &);
    void OnSetCurPage(const QString &sValue);

private slots:
    void slotFileChange(const QString &);
    void slotHistroyChange(const QString &);
    void slotAppExitNothing(const QString &);

private:
    QString m_strCurrentFilePath = "";              //  当前显示的文档路径
    QMap<QString, FileDataModel>    m_pFileStateMap;        //  已打开的文档列表
};

#endif // DATAMANAGER_H

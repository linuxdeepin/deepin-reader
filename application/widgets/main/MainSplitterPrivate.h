#ifndef MAINTABWIDGETEXPRIVATE_H
#define MAINTABWIDGETEXPRIVATE_H

#include <QObject>
#include <QMap>

#include "application.h"
#include "FileDataModel.h"

/**
 * @brief The DataManager class
 * @brief   固定数据管理区， 采用 单例
 */

class MainSplitter;

class MainSplitterPrivate : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(MainSplitterPrivate)

public:
    explicit MainSplitterPrivate(MainSplitter *parent = nullptr);

    // IObserver interface
public:
    int qDealWithData(const int &, const QString &);

    void qSetFileChange(const int &);
    int qGetFileChange();

    FileDataModel qGetFileData() const;
    void qSetFileData(const FileDataModel &);

    QString qGetPath() const;
    void qSetPath(const QString &strPath);

private:
    void setThumbnailState(const QString &);
    void SetLeftWidgetIndex(const QString &sValue);
    void OnSetViewChange(const QString &);
    void OnSetViewScale(const QString &);
    void OnSetViewRotate(const QString &);
    void OnSetViewHit(const QString &);
    void OnSetCurPage(const QString &sValue);

    void slotFileChange(const QString &);
    void slotHistroyChange(const QString &);

private:
    QString             m_strPath = "";
    FileDataModel       m_pFileDataModel;        //  已打开的文档列表
    int                 m_pChangeState = -1;

    MainSplitter        *q_ptr = nullptr;
};

#endif // DATAMANAGER_H

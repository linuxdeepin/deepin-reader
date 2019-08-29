#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>

/**
 * @brief The DataManager class
 * @brief   固定数据管理区， 采用 单例
 */


class DataManager : public QObject
{
    Q_OBJECT
private:
    DataManager(QObject *parent=nullptr);

public:
    static DataManager* instance()
    {
        static DataManager manager;
        return  &manager;
    }

    QString strOnlyFilePath() const;
    void setStrOnlyFilePath(const QString &strOnlyFilePath);

private:
    QString     m_strOnlyFilePath;      //  只显示一个pdf 文件

};

#endif // DATAMANAGER_H

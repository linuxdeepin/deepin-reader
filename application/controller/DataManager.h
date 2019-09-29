#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>

/**
 * @brief The DataManager class
 * @brief   固定数据管理区， 采用 单例
 */

//  文档显示状态
enum File_Show_Enum {
    FILE_NORMAL,        //  正常显示
    FILE_FULLSCREEN,    //  全屏
    FILE_SLIDE,         //  幻灯片
};

class DataManager : public QObject
{
    Q_OBJECT
private:
    DataManager(QObject *parent = nullptr);

public:
    static DataManager *instance()
    {
        static DataManager manager;
        return &manager;
    }

    QString strOnlyFilePath() const;
    void setStrOnlyFilePath(const QString &strOnlyFilePath);

    int CurShowState() const;
    void setCurShowState(int nCurShowState);

private:
    QString m_strOnlyFilePath; //  只显示一个pdf 文件
    int     m_nCurShowState = -1;   //  文档当前显示状态
};

#endif // DATAMANAGER_H

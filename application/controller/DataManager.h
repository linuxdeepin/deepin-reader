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

    bool bIsUpdate() const;
    void setBIsUpdate(bool bIsUpdate);

    bool bIsBookMarkState() const;
    void setBIsBookMarkState(bool bIsBookMarkState);

    inline int stackWidgetIndex() const
    {
        return m_nStackWidgetIndex;
    }

    inline void setStackWidgetIndex(const int &index)
    {
        m_nStackWidgetIndex = index;
    }

private:
    QString m_strOnlyFilePath; //  只显示一个pdf 文件
    int     m_nCurShowState = -1;   //  文档当前显示状态
    bool    m_bIsUpdate = false;    //  文档是否有修改
    bool    m_bIsBookMarkState = false;    //  当前页的书签状态
    int     m_nStackWidgetIndex = -1;      //  左侧缩略图stack widget当前index，0：所有缩略图1：书签缩略图2：注释缩略图3：搜索缩略图
};

#endif // DATAMANAGER_H

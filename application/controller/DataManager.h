#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QMap>
#include <QKeySequence>
#include <QDebug>

#include "FileData.h"


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


enum ICON_RADIUS {
    ICON_SMALL = 8,     // 小图标圆角
    ICON_BIG = 10       // 大图标圆角
};

class DataManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DataManager)

public:
    explicit DataManager(QObject *parent = nullptr);

    void initKeyList();

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

    QList<QColor>   getLightColorList();

    inline void mousePressLocal(bool &highLight, QPoint &point)
    {
        highLight = m_bIsHighLight;
        point = m_point;
    }
    void setMousePressLocal(const bool &highLight, const QPoint &point);

    inline void setScreenRect(const QRect &rect)
    {
        m_screenRect = rect;
    }

    inline void setSmallNoteWidgetSize(const QSize &size)
    {
        m_smallNoteSize = size;
    }

    QString gettrCurrentTheme() const;
    void settrCurrentTheme(const QString &strCurrentTheme);

    inline QColor selectColor()
    {
        return m_selectColor;
    }
    inline void setSelectColor(const QColor &color)
    {
        m_selectColor = color;
    }

    inline bool bThumbnIsShow() const
    {
        return m_bThumbnIsShow;
    }
    inline void setBThumbnIsShow(const bool show)
    {
        m_bThumbnIsShow = show;
    }

    QList<QKeySequence> getPKeyList() const;

    QList<QKeySequence> getPLeftKeyList() const;

    FileData qGetFileData(const QString &) const;
    void qSetFileData(const QString &strPath, const FileData &);
    QMap<QString, FileData> qGetFileStateMap() const;

private:
    QMap<QString, FileData> m_pFileStateMap;     //  已打开的文档列表

    QString m_strCurrentTheme = "";     //  当前主题
    QString m_strOnlyFilePath = "";     //  只显示一个pdf 文件
    int     m_nCurShowState = FILE_NORMAL;       //  文档当前显示状态
    bool    m_bIsUpdate = false;        //  文档是否有修改
    QList<QColor> m_listColor;          //  color list
    bool m_bIsHighLight = false;        // 鼠标左键点击位置有没有高亮
    QPoint m_point;                     // 鼠标左键点击位置
    QRect m_screenRect;                 // 屏幕的分辨率
    QSize m_smallNoteSize;              // 注释小窗体的大小
    QColor m_selectColor;               // 高亮颜色
    bool m_bThumbnIsShow = false;       // 左侧缩略图是否展开
    bool m_bFirstOpenFile = false;      // 初次打开某个文档
    QList<QKeySequence>     m_pKeyList;      //  快捷键对应
    QList<QKeySequence>     m_pLeftKeyList;      // left 快捷键对应
};

#endif // DATAMANAGER_H

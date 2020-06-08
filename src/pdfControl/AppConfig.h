#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QKeySequence>
#include <QSize>
#include <QSettings>


enum ICON_RADIUS {
    ICON_SMALL = 8,     // 小图标圆角
    ICON_BIG = 10       // 大图标圆角
};

enum SET_KEY {
    KEY_APP_WIDTH,
    KEY_APP_HEIGHT
};


class AppConfig : public QObject
{
    Q_OBJECT
public:
    explicit AppConfig(QObject *parent = nullptr);

private:
    void InitColor();

    void InitKeyList();

public:
    QColor selectColor() const;

    void setSelectColor(const QColor &color);

    QList<QColor>   getLightColorList();

    QList<QKeySequence> getKeyList() const;

    void mousePressLocal(bool &highLight, QPoint &point);

    void setMousePressLocal(const bool &highLight, const QPoint &point);

    void setScreenRect(const QRect &rect);

    void setSmallNoteWidgetSize(const QSize &size);

public:
    void setAppKeyValue(const int &, const QString &);

    QString getAppKeyValue(const int &) const;

private:
    QList<QKeySequence>     m_pKeyList;                 //  快捷键对应
    QList<QColor>           m_listColor;                //  color list
    QColor                  m_selectColor;              // 高亮颜色
    bool m_bIsHighLight = false;                        // 鼠标左键点击位置有没有高亮
    QPoint m_point;                                     // 鼠标左键点击位置
    QRect m_screenRect;                                 // 屏幕的分辨率
    QSize m_smallNoteSize;                              // 注释小窗体的大小
    QSettings *m_pSettings = nullptr;
};

#endif // APPCONFIG_H

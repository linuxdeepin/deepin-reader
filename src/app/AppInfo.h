#ifndef APPINFO_H
#define APPINFO_H

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QKeySequence>
#include <QSize>
#include <QSettings>

enum SET_KEY {
    KEY_APP_WIDTH,
    KEY_APP_HEIGHT
};

class AppInfo: public QObject
{
    Q_OBJECT
public:
    explicit AppInfo(QObject *parent = nullptr);

private:
    void InitColor();

public:
    QList<QColor>   getLightColorList();

    QRect screenRect()const;

    void setScreenRect(const QRect &rect);

public:
    void setAppKeyValue(const int &, const QString &);

    QString getAppKeyValue(const int &) const;

private:
    QList<QColor>           m_listColor;                //  color list
    QRect m_screenRect;                                 // 屏幕的分辨率
    QSettings *m_pSettings = nullptr;
};

#endif // APPINFO_H

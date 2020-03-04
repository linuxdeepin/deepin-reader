/*
 * Copyright (C) 2019 ~ 2020 UOS Technology Co., Ltd.
 *
 * Author:     wangzhxiaun
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef APPINFO_H
#define APPINFO_H

#include <QObject>
#include <QColor>
#include <QPoint>
#include <QRect>
#include <QKeySequence>
#include <QSize>
#include <QSettings>

//  文档显示状态
//enum File_Show_Enum {
//    FILE_NORMAL,        //  正常显示
//    FILE_FULLSCREEN,    //  全屏
//    FILE_SLIDE,         //  幻灯片
//};


enum ICON_RADIUS {
    ICON_SMALL = 8,     // 小图标圆角
    ICON_BIG = 10       // 大图标圆角
};

enum SET_KEY {
    KEY_APP_WIDTH,
    KEY_APP_HEIGHT
};


class AppInfo : public QObject
{
    Q_OBJECT
public:
    explicit AppInfo(QObject *parent = nullptr);

private:
    void InitColor();
    void InitKeyList();

public:
    QColor selectColor() const;
    void setSelectColor(const QColor &color);

    QList<QColor>   getLightColorList();

    QList<QKeySequence> getKeyList() const;

    QString qGetCurrentTheme() const;
    void qSetCurrentTheme(const QString &strCurrentTheme);

    int qGetCurShowState() const;
    void qSetCurShowState(const int &nState);

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
    QString m_strCurrentTheme = "";                     //  当前主题
//    int     m_nCurShowState = FILE_NORMAL;              //  文档当前显示状态
    bool m_bIsHighLight = false;                        // 鼠标左键点击位置有没有高亮
    QPoint m_point;                                     // 鼠标左键点击位置
    QRect m_screenRect;                                 // 屏幕的分辨率
    QSize m_smallNoteSize;                              // 注释小窗体的大小

    QSettings *m_pSettings = nullptr;
};

#endif // CONSTANTHEADER_H

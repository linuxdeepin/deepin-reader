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
#include <QKeySequence>

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


private:
    QList<QKeySequence>     m_pKeyList;                 //  快捷键对应
    QList<QColor>           m_listColor;                //  color list
    QColor                  m_selectColor;              // 高亮颜色
    QString m_strCurrentTheme = "";     //  当前主题

};

#endif // CONSTANTHEADER_H

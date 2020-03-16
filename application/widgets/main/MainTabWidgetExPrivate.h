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
#ifndef MAINTABWIDGETEXPRIVATE_H
#define MAINTABWIDGETEXPRIVATE_H

#include <QObject>
#include <QMap>

#include "ModuleHeader.h"

class FileDataModel;
class MainTabWidgetEx;

class MainTabWidgetExPrivate : public QObject
{
public:
    MainTabWidgetExPrivate(MainTabWidgetEx *);

public:
    void OpenCurFileFolder();

    QString GetCurPath();
    QStringList GetAllPath();

    int GetFileChange(const QString &sPath);
    void SetFileChange(const QString &sPath, const int &iState);

private:
    QList<int>          m_pMsgList;

    QString             m_strSliderPath = "";
    QString             m_strMagniferPath = "";
    int                 m_nCurrentState = Default_State;

private:
    MainTabWidgetEx *const q_ptr = nullptr;
    Q_DECLARE_PUBLIC(MainTabWidgetEx)
};

#endif // MAINTABWIDGETEXPRIVATE_H

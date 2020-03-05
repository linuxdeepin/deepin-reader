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

class DocummentProxy;
class FileDataModel;
class MainTabWidgetEx;

class MainTabWidgetExPrivate
{
public:
    explicit MainTabWidgetExPrivate(MainTabWidgetEx *widget);

public:
    QString getSliderPath() const;
    void setSliderPath(const QString &strSliderPath);

    int getCurrentState() const;
    void setCurrentState(const int &nCurrentState);

    void RemovePath(const QString &);
    void InsertPathProxy(const QString &, DocummentProxy *);
    DocummentProxy *getCurFileAndProxy(const QString &sPath = "") const;

    QMap<QString, DocummentProxy *> getOpenFileAndProxy() const;

    void OpenCurFileFolder();

    QString GetCurPath() const;
    QStringList GetAllPath() const;

    int GetFileChange(const QString &sPath);
    void SetFileChange(const QString &sPath, const int &iState);

    void SetFileData(const QString &sPath, const FileDataModel &fdm);
    FileDataModel GetFileData(const QString &sPath) const;

    QString getMagniferPath() const;
    void setMagniferPath(const QString &strMagniferPath);

private:
    QMap<QString, DocummentProxy *> m_strOpenFileAndProxy;

    QString             m_strSliderPath = "";
    QString             m_strMagniferPath = "";
    int                 m_nCurrentState = -1;


    MainTabWidgetEx     *q_ptr = nullptr;

};

#endif // MAINTABWIDGETEXPRIVATE_H

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
#ifndef PROXYDATA_H
#define PROXYDATA_H

#include <QObject>
#include <QPoint>

class ProxyData : public QObject
{
    Q_OBJECT
public:
    explicit ProxyData(QObject *parent = nullptr);

    QString getPath() const;
    void setPath(const QString &strPath);

    bool IsFileChanged() const;
    void setFileChanged(bool filechanged);

    bool IsHandleSelect() const;
    void setIsHandleSelect(bool bIsHandleSelect);

    QPoint getStartPoint() const;
    void setStartPoint(const QPoint &pStartPoint);

    QPoint getEndSelectPoint() const;
    void setEndSelectPoint(const QPoint &pEndSelectPoint);

    bool IsFirstShow() const;
    void setFirstShow(bool bFirstShow);

    bool getIsFileOpenOk() const;
    void setIsFileOpenOk(bool bIsFileOpenOk);

private:
    bool                m_bIsFileOpenOk = false;
    QString             m_strPath = "";
    bool                m_filechanged = false;
    bool                m_bIsHandleSelect = false;              //  是否可以选中

    QPoint              m_pStartPoint;
    QPoint              m_pEndSelectPoint;

    bool                m_bFirstShow = true;        //  是否是第一次显示,  用于判断 resizeEvent
};

#endif // PROXYDATA_H

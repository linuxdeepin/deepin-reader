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
#include "ProxyData.h"
#include "TitleMenu.h"

ProxyData::ProxyData(QObject *parent)
    : QObject(parent)
{

}

QString ProxyData::getPath() const
{
    return m_strPath;
}

void ProxyData::setPath(const QString &strPath)
{
    m_strPath = strPath;
}

bool ProxyData::IsFileChanged() const
{
    return m_filechanged;
}

void ProxyData::setFileChanged(bool filechanged)
{
    m_filechanged = filechanged;
    emit signale_filechanged(filechanged);
}

bool ProxyData::IsHandleSelect() const
{
    return m_bIsHandleSelect;
}

void ProxyData::setIsHandleSelect(bool bIsHandleSelect)
{
    m_bIsHandleSelect = bIsHandleSelect;
}

QPoint ProxyData::getStartPoint() const
{
    return m_pStartPoint;
}

void ProxyData::setStartPoint(const QPoint &pStartPoint)
{
    m_pStartPoint = pStartPoint;
}

QPoint ProxyData::getEndSelectPoint() const
{
    return m_pEndSelectPoint;
}

void ProxyData::setEndSelectPoint(const QPoint &pEndSelectPoint)
{
    m_pEndSelectPoint = pEndSelectPoint;
}

bool ProxyData::IsFirstShow() const
{
    return m_bFirstShow;
}

void ProxyData::setFirstShow(bool bFirstShow)
{
    m_bFirstShow = bFirstShow;
}

bool ProxyData::getIsFileOpenOk() const
{
    return m_bIsFileOpenOk;
}

void ProxyData::setIsFileOpenOk(bool bIsFileOpenOk)
{
    m_bIsFileOpenOk = bIsFileOpenOk;
}

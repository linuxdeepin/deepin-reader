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
#include "DBService.h"

#include "BookMarkDB.h"
#include "HistroyDB.h"

DBService::DBService(QObject *parent)
    : QObject(parent)
{
    m_pBookMark = new BookMarkDB(this);
    m_pHistroy = new HistroyDB(this);
}

void DBService::qSetStrFilePath(const QString &strFilePath)
{
    m_pBookMark->setStrFilePath(strFilePath);
    m_pHistroy->setStrFilePath(strFilePath);
}

void DBService::qSelectData(const int &iType)
{
    if (iType == DB_BOOKMARK) {
        m_pBookMark->qSelectData();
    } else {
        m_pHistroy->qSelectData();
    }
}

void DBService::qSaveData(const QString &sPath, const int &iType)
{
    if (iType == DB_BOOKMARK) {
        m_pBookMark->saveData(sPath);
    } else {
        m_pHistroy->saveData(sPath);
    }
}

QList<int> DBService::getBookMarkList() const
{
    return qobject_cast<BookMarkDB *>(m_pBookMark)->getBookMarkList();
}

void DBService::setBookMarkList(const QList<int> &pBookMarkList)
{
    qobject_cast<BookMarkDB *>(m_pBookMark)->setBookMarkList(pBookMarkList);
}

QJsonObject DBService::getHistroyData() const
{
    return qobject_cast<HistroyDB *>(m_pHistroy)->getHistroyData();
}

void DBService::setHistroyData(const QString &sKey, const int &iValue)
{
    qobject_cast<HistroyDB *>(m_pHistroy)->setHistroyData(sKey, iValue);
}

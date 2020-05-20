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

void DBService::qSelectData(const QString &sPath, const int &type)
{
    if (type == DB_BOOKMARK) {
        m_pBookMark->qSelectData(sPath);
    } else {
        m_pHistroy->qSelectData(sPath);
    }
}

void DBService::qSaveData(const QString &sPath, const int &type)
{
    if (type == DB_BOOKMARK) {
        m_pBookMark->saveData(sPath);
    } else {
        m_pHistroy->saveData(sPath);
    }
}

void DBService::saveAsData(const QString &originPath, const QString &targetPath, const int &type)
{
    if (type == DB_BOOKMARK) {
        m_pBookMark->saveAsData(originPath, targetPath);
    }
}

QList<int> DBService::getBookMarkList(const QString &sPath) const
{
    return qobject_cast<BookMarkDB *>(m_pBookMark)->getBookMarkList(sPath);
}

void DBService::setBookMarkList(const QString &sPath, const QList<int> &pBookMarkList)
{
    qobject_cast<BookMarkDB *>(m_pBookMark)->setBookMarkList(sPath, pBookMarkList);
}

FileDataModel DBService::getHistroyData(const QString &sPath) const
{
    return qobject_cast<HistroyDB *>(m_pHistroy)->getHistroyData(sPath);
}

void DBService::setHistroyData(const QString &sPath, const int &iKey, const QVariant &iValue)
{
    qobject_cast<HistroyDB *>(m_pHistroy)->setHistroyData(sPath, iKey, iValue);
}


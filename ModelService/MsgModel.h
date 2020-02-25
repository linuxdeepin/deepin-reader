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
#ifndef MSGMODEL_H
#define MSGMODEL_H

#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QString>

class MsgModel
{
public:
    QString toJson();
    void fromJson(const QString &sText);

    QString getShortKey() const;
    void setShortKey(const QString &value);

    QString getPath() const;
    void setPath(const QString &value);

    int getMsgType() const;
    void setMsgType(int value);

    QString getValue() const;
    void setValue(const QString &value);

private:
    QString     sShortKey = "";
    QString     sPath = "";
    int         msgType = -1;
    QString     sValue = "";
};

#endif // MSGMODEL_H



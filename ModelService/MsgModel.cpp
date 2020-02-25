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

#include "MsgModel.h"

QString MsgModel::toJson()
{
    QJsonObject obj;
    obj.insert("key", sShortKey);
    obj.insert("path", sPath);
    obj.insert("msgType", msgType);
    obj.insert("sValue", sValue);

    QJsonDocument doc(obj);

    return doc.toJson(QJsonDocument::Compact);

}

void MsgModel::fromJson(const QString &sText)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(sText.toLocal8Bit().data(), &error);
    if (error.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        sShortKey = obj["key"].toString();
        sPath = obj["path"].toString();
        msgType = obj["msgType"].toInt();
        sValue = obj["sValue"].toString();
    }
}

QString MsgModel::getShortKey() const
{
    return sShortKey;
}

void MsgModel::setShortKey(const QString &value)
{
    sShortKey = value;
}

QString MsgModel::getPath() const
{
    return sPath;
}

void MsgModel::setPath(const QString &value)
{
    sPath = value;
}

int MsgModel::getMsgType() const
{
    return msgType;
}

void MsgModel::setMsgType(int value)
{
    msgType = value;
}

QString MsgModel::getValue() const
{
    return sValue;
}

void MsgModel::setValue(const QString &value)
{
    sValue = value;
}

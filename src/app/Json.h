/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     zhangsong<zhangsong@uniontech.com>
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#ifndef JSON_H
#define JSON_H

#include <QJsonObject>
#include <QJsonDocument>

struct JsonPrivate;
class Json
{
public:
    explicit Json(const QString &jsonOrJsonFilePath = "{}", bool fromFile = false);
    ~Json();
    int         getInt(const QString &path, int def = 0, const QJsonObject &fromNode = QJsonObject()) const;
    bool        getBool(const QString &path, bool def = false, const QJsonObject &fromNode = QJsonObject()) const;
    double      getDouble(const QString &path, double def = 0.0, const QJsonObject &fromNode = QJsonObject()) const;
    QString     getString(const QString &path, const QString &def = QString(), const QJsonObject &fromNode = QJsonObject()) const;
    QStringList getStringList(const QString &path, const QJsonObject &fromNode = QJsonObject()) const;
    QJsonArray  getJsonArray(const QString &path, const QJsonObject &fromNode = QJsonObject()) const;
    QJsonValue  getJsonValue(const QString &path, const QJsonObject &fromNode = QJsonObject()) const;
    QJsonObject getJsonObject(const QString &path, const QJsonObject &fromNode = QJsonObject()) const;
    void set(const QString &path, const QJsonValue &value);
    void set(const QString &path, const QStringList &strings);
    void save(const QString &path, QJsonDocument::JsonFormat format = QJsonDocument::Indented);
    QString toString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const;
public:
    JsonPrivate *d;
};
#endif // JSON_H

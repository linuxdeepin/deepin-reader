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

#include "FileDataModel.h"
#include <QJsonObject>
#include <QJsonDocument>

QString FileDataModel::toJson()
{
    QJsonObject obj;
    obj.insert("thumbnail", bThumbnail);
    obj.insert("doubleShow", bDoubleShow);
    obj.insert("fit", nFit);
    obj.insert("rotate", nRotate);
    obj.insert("scale", nScale);
    obj.insert("leftIndex", nLeftIndex);
    obj.insert("curPage", nCurPage);

    QJsonDocument doc(obj);

    return doc.toJson(QJsonDocument::Compact);
}

void FileDataModel::fromJson(const QString &sText)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(sText.toLocal8Bit().data(), &error);
    if (error.error == QJsonParseError::NoError) {
        QJsonObject obj = doc.object();
        bThumbnail = obj["thumbnail"].toBool();
        bDoubleShow = obj["doubleShow"].toBool();
        nFit = obj["fit"].toInt();
        nRotate = obj["rotate"].toInt();
        nScale = obj["scale"].toInt();
        nLeftIndex = obj["leftIndex"].toInt();
        nCurPage = obj["curPage"].toInt();
    }
}

void FileDataModel::qSetData(const int &iKey, const double &iValue)
{
    if (iKey == Thumbnail) {
        setThumbnail(iValue);
    } else if (iKey == DoubleShow) {
        setDoubleShow(iValue);
    } else if (iKey == Fit) {
        setFit(iValue);
    } else if (iKey == Rotate) {
        setRotate(iValue);
    } else if (iKey == Scale) {
        setScale(iValue);
    } else if (iKey == LeftIndex) {
        setLeftIndex(iValue);
    } else if (iKey == CurPage) {
        setCurPage(iValue);
    }
}

double FileDataModel::qGetData(const int &iKey) const
{
    if (iKey == Thumbnail) {
        return getThumbnail();
    }
    if (iKey == DoubleShow) {
        return getDoubleShow();
    }
    if (iKey == Fit) {
        return getFit();
    }
    if (iKey == Rotate) {
        return getRotate();
    }
    if (iKey == Scale) {
        return  getScale();
    }
    if (iKey == LeftIndex) {
        return getLeftIndex();
    }
    if (iKey == CurPage) {
        return getCurPage();
    }
    return -1;
}

bool FileDataModel::getThumbnail() const
{
    return bThumbnail;
}

void FileDataModel::setThumbnail(bool value)
{
    bThumbnail = value;
}

bool FileDataModel::getDoubleShow() const
{
    return bDoubleShow;
}

void FileDataModel::setDoubleShow(bool value)
{
    bDoubleShow = value;
}

int FileDataModel::getFit() const
{
    return nFit;
}

void FileDataModel::setFit(int value)
{
    nFit = value;
}

int FileDataModel::getRotate() const
{
    return nRotate;
}

void FileDataModel::setRotate(int value)
{
    nRotate = value;
}

double FileDataModel::getScale() const
{
    return nScale;
}

void FileDataModel::setScale(double value)
{
    nScale = value;
}

int FileDataModel::getLeftIndex() const
{
    return nLeftIndex;
}

void FileDataModel::setLeftIndex(int value)
{
    nLeftIndex = value;
}

int FileDataModel::getCurPage() const
{
    return nCurPage;
}

void FileDataModel::setCurPage(int value)
{
    nCurPage = value;
}

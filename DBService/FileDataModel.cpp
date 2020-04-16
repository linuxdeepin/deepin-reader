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

#include <QDebug>

void FileDataModel::setOper(const int &iKey, const QVariant &iValue)
{
    if (iKey == Thumbnail) {
        setThumbnail(iValue.toInt());
    } else if (iKey == DoubleShow) {
        setDoubleShow(iValue.toInt());
    } else if (iKey == Fit) {
        setFit(iValue.toInt());
    } else if (iKey == Rotate) {
        setRotate(iValue.toInt());
    } else if (iKey == Scale) {
        setScale(iValue.toDouble());
    } else if (iKey == LeftIndex) {
        setLeftIndex(iValue.toInt());
    } else if (iKey == CurPage) {
        setCurPage(iValue.toInt());
    } else if (iKey == HandShape) {
        setCurMouse(iValue.toInt());
    }
}

QVariant FileDataModel::getOper(const int &iKey) const
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
    if (iKey == HandShape) {
        return getCurMouse();
    }
    return -1;
}

int FileDataModel::getThumbnail() const
{
    return bThumbnail;
}

void FileDataModel::setThumbnail(int value)
{
    bThumbnail = value;
}

int FileDataModel::getDoubleShow() const
{
    return bDoubleShow;
}

void FileDataModel::setDoubleShow(int value)
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

int FileDataModel::getCurMouse() const
{
    return nCurMouse;
}

void FileDataModel::setCurMouse(int value)
{
    nCurMouse = value;
}

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
#ifndef FILEDATAMODEL_H
#define FILEDATAMODEL_H

#include <QString>
#include <QVariant>

enum E_FDM_MSG {
    Path = 0,
    Scale,
    DoubleShow,
    Fit,
    Rotate,
    Thumbnail,
    LeftIndex,
    CurPage,
    HandShape
};


/**
 * @brief       文件状态\数据 模型
 *
 */

class FileDataModel
{
public:
    void     setOper(const int &iKey, const QVariant &value);
    QVariant getOper(const int &iKey) const;

public:
    int  getThumbnail() const;
    void setThumbnail(int value);

    int  getDoubleShow() const;
    void setDoubleShow(int value);

    int getFit() const;
    void setFit(int value);

    int getRotate() const;
    void setRotate(int value);

    double getScale() const;
    void setScale(double value);

    int getLeftIndex() const;
    void setLeftIndex(int value);

    int getCurPage() const;
    void setCurPage(int value);

    int getCurMouse() const;
    void setCurMouse(int value);

private:
    int        bThumbnail = 0;
    int        bDoubleShow = 0;
    int        nFit = 0;
    int        nRotate = 0;
    double     nScale = 0;
    int        nLeftIndex = 0;
    int        nCurPage = 0;
    int        nCurMouse = 0;
};

#endif // FILEDATA_H

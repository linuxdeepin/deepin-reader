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


enum E_FDM_MSG {
    Path = 0,
    Scale,
    DoubleShow,
    Fit,
    Rotate,
    Thumbnail,
    LeftIndex,
    CurPage
};


/**
 * @brief       文件状态\数据 模型
 *
 */

class FileDataModel
{
public:
    QString toJson();
    void fromJson(const QString &sText);

public:
    void qSetData(const int &iKey, const int &iValue);
    int qGetData(const int &iKey) const;

private:
    bool getThumbnail() const;
    void setThumbnail(bool value);

    bool getDoubleShow() const;
    void setDoubleShow(bool value);

    int getFit() const;
    void setFit(int value);

    int getRotate() const;
    void setRotate(int value);

    int getScale() const;
    void setScale(int value);

    int getLeftIndex() const;
    void setLeftIndex(int value);

    int getCurPage() const;
    void setCurPage(int value);

private:
    bool        bThumbnail = false;
    bool        bDoubleShow = false;
    int         nFit = false;
    int         nRotate = -1;
    int         nScale = -1;
    int         nLeftIndex = 0;
    int         nCurPage = -1;
};

#endif // FILEDATA_H

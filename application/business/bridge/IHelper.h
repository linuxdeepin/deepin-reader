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
#ifndef IHELPER_H
#define IHELPER_H

#include <QString>

class HelperImpl;

class IHelper
{
protected:
    IHelper();

public:
    virtual ~IHelper() {}
    virtual void qDealWithData(const int &, const QString &) = 0;
};

class Helper : public IHelper
{
public:
    Helper();
    void qDealWithData(const int &msgType, const QString &msgContent);

private:
    HelperImpl  *m_pAnnotatinHelperImpl = nullptr;
    HelperImpl  *m_pDocHelperImpl = nullptr;
};


#endif // IHELPER_H

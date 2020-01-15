/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     duanxiaohui
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
#ifndef ABSTRACTION_H
#define ABSTRACTION_H


#include <QString>
#include <QObject>
#include <QDebug>

#include "AbstractImplement.h"

class AbstractionImplement;

class Abstraction
{
public:
    virtual ~Abstraction();
    virtual void Operation(const QString &) = 0;

protected:
    Abstraction();
};

class RefinedAbstractionA : public QObject, public Abstraction
{
public:
    explicit RefinedAbstractionA(AbstractionImplement *imp); //构造函数
    ~RefinedAbstractionA() Q_DECL_OVERRIDE;//析构函数
    void Operation(const QString &) Q_DECL_OVERRIDE;//实现接口
private:
    AbstractionImplement *_imp;//私有成员

};

#endif // ABSTRACTION_H

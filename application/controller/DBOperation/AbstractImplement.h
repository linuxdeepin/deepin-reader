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
#ifndef ABSTRACTIMPLEMENT_H
#define ABSTRACTIMPLEMENT_H

#include <QString>
#include <QObject>
#include <QDebug>

class AbstractionImplement
{
public:
    virtual ~AbstractionImplement();
    virtual void Operation(const QString &) = 0; //定义操作接口

protected:
    AbstractionImplement();
};

// 继承自AbstractionImplement,是AbstractionImplement的不同实现之一
class ConcreteAbstractionImplementA : public QObject, public AbstractionImplement
{
    Q_OBJECT
    Q_DISABLE_COPY(ConcreteAbstractionImplementA)
public:
    explicit ConcreteAbstractionImplementA(QObject *obj = nullptr);
    ~ConcreteAbstractionImplementA() Q_DECL_OVERRIDE;
    void Operation(const QString &) Q_DECL_OVERRIDE;//实现操作
protected:
};

#endif // ABSTRACTIMPLEMENT_H

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
#include "Abstraction.h"

Abstraction::Abstraction()
{

}

Abstraction::~Abstraction()
{

}

RefinedAbstractionA::RefinedAbstractionA(AbstractionImplement *imp):
    _imp(imp)
{

}

RefinedAbstractionA::~RefinedAbstractionA()
{
    if (_imp) {
        delete _imp;
        _imp = nullptr;
    }
}

void RefinedAbstractionA::Operation(const QString &action)
{
    qDebug() << action;
}


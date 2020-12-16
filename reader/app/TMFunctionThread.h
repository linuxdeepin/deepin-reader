/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     leiyu <leiyu@uniontech.com>
*
* Maintainer: leiyu <leiyu@uniontech.com>
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
#ifndef TMFUNCTIONTHREAD_H
#define TMFUNCTIONTHREAD_H

#include <QThread>
#include <QVariant>

#include <functional>

typedef std::function<QVariant(void)> FNTemplate;
class TMFunctionThread : public QThread
{
    Q_OBJECT
public:
    explicit TMFunctionThread(QObject *parent = nullptr);
    ~TMFunctionThread();

public:
    FNTemplate func;
    QVariant result;

protected:
    void run() override;
};

#endif // TMFUNCTIONTHREAD_H

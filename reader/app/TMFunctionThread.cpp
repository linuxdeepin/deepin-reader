// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TMFunctionThread.h"

TMFunctionThread::TMFunctionThread(QObject *parent) : QThread(parent)
{
    qDebug() << "Function thread created";
}

TMFunctionThread::~TMFunctionThread()
{
    qDebug() << "Destroying function thread";
    this->wait();
    qDebug() << "Function thread destroyed";
}

void TMFunctionThread::run()
{
    qDebug() << "Function thread started running";
    result = func();
    qDebug() << "Function execution completed, result:" << result;
}

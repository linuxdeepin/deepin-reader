// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "TMFunctionThread.h"
#include "ddlog.h"

#include <QDebug>

TMFunctionThread::TMFunctionThread(QObject *parent) : QThread(parent)
{
    qCDebug(appLog) << "Function thread created";
}

TMFunctionThread::~TMFunctionThread()
{
    qCDebug(appLog) << "Destroying function thread";
    this->wait();
    qCDebug(appLog) << "Function thread destroyed";
}

void TMFunctionThread::run()
{
    qCDebug(appLog) << "Function thread started running";
    result = func();
    qCDebug(appLog) << "Function execution completed, result:" << result;
}

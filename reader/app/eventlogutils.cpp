// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QLibrary>
#include <QDir>
#include <QLibraryInfo>
#include <QJsonDocument>

#include "eventlogutils.h"

EventLogUtils *EventLogUtils::mInstance(nullptr);

EventLogUtils &EventLogUtils::get()
{
    if (mInstance == nullptr) {
        qDebug() << "Creating new EventLogUtils instance";
        mInstance = new EventLogUtils;
    }
    qDebug() << "Returning EventLogUtils instance";
    return *mInstance;
}

EventLogUtils::EventLogUtils()
{
    qDebug() << "Loading event log library";
    QLibrary library("libdeepin-event-log.so");

    qDebug() << "Resolving Initialize function";
    init = reinterpret_cast<bool (*)(const std::string &, bool)>(library.resolve("Initialize"));
    qDebug() << "Resolving WriteEventLog function";
    writeEventLog = reinterpret_cast<void (*)(const std::string &)>(library.resolve("WriteEventLog"));

    if (init == nullptr) {
        qWarning() << "Failed to resolve Initialize function";
        return;
    }

    qDebug() << "Initializing event log for deepin-reader";
    if (!init("deepin-reader", true)) {
        qWarning() << "Failed to initialize event log";
    } else {
        qDebug() << "Event log initialized successfully";
    }
}

void EventLogUtils::writeLogs(QJsonObject &data)
{
    if (writeEventLog == nullptr) {
        qWarning() << "WriteEventLog function not available";
        return;
    }

    qDebug() << "Writing event log data";
    QByteArray jsonData = QJsonDocument(data).toJson(QJsonDocument::Compact);
    qDebug() << "Event log content:" << jsonData;
    writeEventLog(jsonData.toStdString());
    qDebug() << "Event log written successfully";
}

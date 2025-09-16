// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QLibrary>
#include <QDir>
#include <QLibraryInfo>
#include <QJsonDocument>

#include "eventlogutils.h"
#include "ddlog.h"

EventLogUtils *EventLogUtils::mInstance(nullptr);

EventLogUtils &EventLogUtils::get()
{
    if (mInstance == nullptr) {
        qCDebug(appLog) << "Creating new EventLogUtils instance";
        mInstance = new EventLogUtils;
    }
    qCDebug(appLog) << "Returning EventLogUtils instance";
    return *mInstance;
}

EventLogUtils::EventLogUtils()
{
    qCDebug(appLog) << "Loading event log library";
    QLibrary library("libdeepin-event-log.so");

    qCDebug(appLog) << "Resolving Initialize function";
    init = reinterpret_cast<bool (*)(const std::string &, bool)>(library.resolve("Initialize"));
    qCDebug(appLog) << "Resolving WriteEventLog function";
    writeEventLog = reinterpret_cast<void (*)(const std::string &)>(library.resolve("WriteEventLog"));

    if (init == nullptr) {
        qCWarning(appLog) << "Failed to resolve Initialize function";
        return;
    }

    qCDebug(appLog) << "Initializing event log for deepin-reader";
    if (!init("deepin-reader", true)) {
        qCWarning(appLog) << "Failed to initialize event log";
    } else {
        qCDebug(appLog) << "Event log initialized successfully";
    }
}

void EventLogUtils::writeLogs(QJsonObject &data)
{
    if (writeEventLog == nullptr) {
        qCWarning(appLog) << "WriteEventLog function not available";
        return;
    }

    qCDebug(appLog) << "Writing event log data";
    QByteArray jsonData = QJsonDocument(data).toJson(QJsonDocument::Compact);
    qCDebug(appLog) << "Event log content:" << jsonData;
    writeEventLog(jsonData.toStdString());
    qCDebug(appLog) << "Event log written successfully";
}

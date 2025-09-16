// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "DebugTimeManager.h"
#include "ddlog.h"

#include <QDateTime>
#include <QDebug>
#include <sys/time.h>

DebugTimeManager    *DebugTimeManager::s_Instance = nullptr;

DebugTimeManager::DebugTimeManager()
{
    qCDebug(appLog) << "DebugTimeManager initialized";
}

void DebugTimeManager::clear()
{
    qCDebug(appLog) << "Clearing all debug time points";
    m_MapPoint.clear();
}

void DebugTimeManager::beginPointQt(const QString &point, const QString &status)
{
    qCDebug(appLog) << "Starting Qt time point:" << point << "status:" << status;
    PointInfo info;
    info.desc = status;
    info.time = QDateTime::currentMSecsSinceEpoch();
    m_MapPoint.insert(point, info);
}
void DebugTimeManager::endPointQt(const QString &point)
{
    if (m_MapPoint.find(point) != m_MapPoint.end()) {
        qint64 elapsed = QDateTime::currentMSecsSinceEpoch() - m_MapPoint[point].time;
        qCDebug(appLog) << "Ending Qt time point:" << point << "elapsed:" << elapsed << "ms";
        m_MapPoint[point].time = elapsed;
        qCInfo(appLog) << QString("[GRABPOINT] %1 %2 time=%3ms").arg(point).arg(m_MapPoint[point].desc).arg(elapsed);
        m_MapPoint.remove(point);
    } else {
        qCWarning(appLog) << "Time point not found:" << point;
    }
}

void DebugTimeManager::beginPointLinux(const QString &point, const QString &status)
{
    qCDebug(appLog) << "Starting Linux time point:" << point << "status:" << status;
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    qint64 currentTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
    qCDebug(appLog) << "Current Linux time:" << currentTime << "ms";

    PointInfo info;
    info.desc = status;
    info.time = currentTime;
    m_MapPoint.insert(point, info);
}
void DebugTimeManager::endPointLinux(const QString &point,  const QString &status)
{
    if (m_MapPoint.find(point) != m_MapPoint.end()) {
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        qint64 currentTime = tv.tv_sec * 1000 + tv.tv_usec / 1000;
        qint64 elapsed = currentTime - m_MapPoint[point].time;
        qCDebug(appLog) << "Ending Linux time point:" << point << "elapsed:" << elapsed << "ms";
        
        m_MapPoint[point].time = elapsed;
        if (!status.isEmpty()) {
            qCDebug(appLog) << "Updating status to:" << status;
            m_MapPoint[point].desc = status;
        }
        qCInfo(appLog) << QString("[GRABPOINT] %1 %2 time=%3ms").arg(point).arg(m_MapPoint[point].desc).arg(elapsed);
        m_MapPoint.remove(point);
    } else {
        qCWarning(appLog) << "Time point not found:" << point;
    }
}

// SPDX-FileCopyrightText: 2025 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DDLOG_H
#define DDLOG_H

#include <QDebug>
#include <QLoggingCategory>

// 全项目统一的日志分类：reader / batch-print / htmltopdf 共用此定义，
// 便于 DConfig(deepin-log-viewer) 按应用统一过滤
inline Q_LOGGING_CATEGORY(appLog, "org.deepin.reader")

// 模块分离：batch-print 批量打印进程
inline Q_LOGGING_CATEGORY(batchprintLog, "org.deepin.reader.batchprint")

// 模块分离：htmltopdf 转换进程
inline Q_LOGGING_CATEGORY(htmltopdfLog, "org.deepin.reader.htmltopdf")

#endif   // DDLOG_H

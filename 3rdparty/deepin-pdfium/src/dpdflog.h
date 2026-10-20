// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DPDFLOG_H
#define DPDFLOG_H

#include <QLoggingCategory>

// deepin-pdfium 日志分类：挂在 org.deepin.reader 前缀下，
// 可被 DConfig(log_rules) 的 org.deepin.reader* 通配规则统一过滤
Q_DECLARE_LOGGING_CATEGORY(dpfLog)

#endif   // DPDFLOG_H

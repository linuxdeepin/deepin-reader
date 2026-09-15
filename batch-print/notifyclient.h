// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NOTIFYCLIENT_H
#define NOTIFYCLIENT_H

#include <QString>
#include <QStringList>

class NotifyClient
{
public:
    static void notifyResult(int total, int succeeded, const QStringList &failedFiles);
    static void notifyError(const QString &body);
    static QString buildBody(int total, int succeeded, const QStringList &failedFiles);
};

#endif // NOTIFYCLIENT_H

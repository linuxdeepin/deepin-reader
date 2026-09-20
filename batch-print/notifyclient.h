// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NOTIFYCLIENT_H
#define NOTIFYCLIENT_H

#include <QDBusMessage>
#include <QString>
#include <QStringList>

class NotifyClient
{
public:
    static void notifyResult(int total, int succeeded, const QStringList &failedFiles);
    static void notifyError(const QString &body);
    static QString buildBody(int total, int succeeded, const QStringList &failedFiles);
    static QDBusMessage buildNotifyMessage(const QString &body);

private:
    static void sendNotification(const QString &body);
};

#endif // NOTIFYCLIENT_H

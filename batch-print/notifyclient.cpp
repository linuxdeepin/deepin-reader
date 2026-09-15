// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "notifyclient.h"
#include "errormessages.h"

#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>
#include <QGuiApplication>

#include <cstdio>

static const int MaxFailedDisplay = 5;

QString NotifyClient::buildBody(int total, int succeeded, const QStringList &failedFiles)
{
    QString body;
    int failed = total - succeeded;

    if (failed == 0) {
        body = ErrorMessages::notifySuccess(total);
    } else if (succeeded == 0) {
        body = ErrorMessages::notifyAllFailed(failed);
    } else {
        body = ErrorMessages::notifyPartialSuccess(succeeded, failed);
    }

    if (failed > 0 && !failedFiles.isEmpty()) {
        QStringList displayList = failedFiles.mid(0, MaxFailedDisplay);
        body += QStringLiteral("\n");
        for (const QString &f : displayList) {
            body += QStringLiteral("\n") + f;
        }
        if (failedFiles.size() > MaxFailedDisplay) {
            body += QStringLiteral("\n") +
                    QGuiApplication::translate("batchprint",
                        "and %1 more file(s) failed.").arg(failedFiles.size() - MaxFailedDisplay);
        }
    }

    return body;
}

void NotifyClient::notifyResult(int total, int succeeded, const QStringList &failedFiles)
{
    QString body = buildBody(total, succeeded, failedFiles);

    QDBusInterface iface(QStringLiteral("org.freedesktop.Notifications"),
                         QStringLiteral("/org/freedesktop/Notifications"),
                         QStringLiteral("org.freedesktop.Notifications"));

    if (!iface.isValid()) {
        fprintf(stderr, "%s\n", body.toUtf8().constData());
        return;
    }

    QVariantList args;
    args << QStringLiteral("deepin-reader");
    args << quint32(0);
    args << QStringLiteral("deepin-reader");
    args << ErrorMessages::notifyTitle();
    args << body;
    args << QStringList();
    args << QVariantMap();
    args << qint32(-1);

    QDBusMessage reply = iface.call(QStringLiteral("Notify"), args);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        fprintf(stderr, "%s\n", body.toUtf8().constData());
    }
}

void NotifyClient::notifyError(const QString &body)
{
    QDBusInterface iface(QStringLiteral("org.freedesktop.Notifications"),
                         QStringLiteral("/org/freedesktop/Notifications"),
                         QStringLiteral("org.freedesktop.Notifications"));

    if (!iface.isValid()) {
        fprintf(stderr, "%s\n", body.toUtf8().constData());
        return;
    }

    QVariantList args;
    args << QStringLiteral("deepin-reader");
    args << quint32(0);
    args << QStringLiteral("deepin-reader");
    args << ErrorMessages::notifyTitle();
    args << body;
    args << QStringList();
    args << QVariantMap();
    args << qint32(-1);

    QDBusMessage reply = iface.call(QStringLiteral("Notify"), args);
    if (reply.type() == QDBusMessage::ErrorMessage) {
        fprintf(stderr, "%s\n", body.toUtf8().constData());
    }
}

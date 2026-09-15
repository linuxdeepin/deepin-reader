// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "errormessages.h"

#include <QGuiApplication>

QString ErrorMessages::cupsUnavailable()
{
    return QGuiApplication::translate("batchprint",
        "CUPS is not available. Please check if the printing service is running.");
}

QString ErrorMessages::noDefaultPrinter()
{
    return QGuiApplication::translate("batchprint",
        "No default printer found. Please set a default printer first.");
}

QString ErrorMessages::convertFailed(const QString &fileName)
{
    return QGuiApplication::translate("batchprint",
        "Failed to convert file: %1").arg(fileName);
}

QString ErrorMessages::printFailed(const QString &fileName)
{
    return QGuiApplication::translate("batchprint",
        "Failed to print file: %1").arg(fileName);
}

QString ErrorMessages::notifyTitle()
{
    return QGuiApplication::translate("batchprint", "Batch Print");
}

QString ErrorMessages::notifySuccess(int count)
{
    return QGuiApplication::translate("batchprint",
        "All %n file(s) printed successfully.", "", count);
}

QString ErrorMessages::notifyPartialSuccess(int succeeded, int failed)
{
    return QGuiApplication::translate("batchprint",
        "%1 file(s) printed successfully, %2 file(s) failed.").arg(succeeded).arg(failed);
}

QString ErrorMessages::notifyAllFailed(int count)
{
    return QGuiApplication::translate("batchprint",
        "All %n file(s) failed to print.", "", count);
}

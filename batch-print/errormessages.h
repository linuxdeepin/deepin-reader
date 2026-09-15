// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ERRORMESSAGES_H
#define ERRORMESSAGES_H

#include <QString>

namespace ErrorMessages {
    QString cupsUnavailable();
    QString noDefaultPrinter();
    QString convertFailed(const QString &fileName);
    QString printFailed(const QString &fileName);
    QString notifyTitle();
    QString notifySuccess(int count);
    QString notifyPartialSuccess(int succeeded, int failed);
    QString notifyAllFailed(int count);
}

#endif // ERRORMESSAGES_H

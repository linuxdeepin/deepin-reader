// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef BATCHPRINTAPP_H
#define BATCHPRINTAPP_H

#include "cupsclient.h"
#include "printsettings.h"

#include <QString>
#include <QStringList>

class BatchPrintApp
{
public:
    explicit BatchPrintApp(ICupsApi *cupsApi = nullptr);
    ~BatchPrintApp();

    int run(const QStringList &fileList);

private:
    ICupsApi *m_cupsApi = nullptr;
    bool m_ownsCupsApi = false;
    PrintSettings m_settings;
};

#endif // BATCHPRINTAPP_H

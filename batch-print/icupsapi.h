// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ICUPSAPI_H
#define ICUPSAPI_H

#include <QString>

#include "printsettings.h"

struct cups_dest_s;
typedef struct cups_dest_s cups_dest_t;
struct cups_option_s;
typedef struct cups_option_s cups_option_t;

class ICupsApi
{
public:
    virtual ~ICupsApi() = default;
    virtual bool getCupsDests(cups_dest_t **dests) = 0;
    virtual bool queryColorSupported(const QString &printerName, bool &supported) = 0;
    virtual bool printFile(const QString &printerName, const QString &filename,
                           const QString &title, int numOptions, cups_option_t *options) = 0;
    virtual bool submitJob(const QString &pdfPath, const QString &title,
                           const PrintSettings &settings) = 0;
};

#endif // ICUPSAPI_H

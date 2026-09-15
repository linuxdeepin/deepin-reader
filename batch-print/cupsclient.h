// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CUPSCLIENT_H
#define CUPSCLIENT_H

#include "icupsapi.h"
#include "printsettings.h"

#include <QLibrary>
#include <QString>

class CupsClient : public ICupsApi
{
public:
    CupsClient();
    ~CupsClient() override;

    bool init();
    bool checkEnvironment();
    bool isColorSupported();
    bool submitJob(const QString &pdfPath, const QString &title, const PrintSettings &settings);

    bool getCupsDests(cups_dest_t **dests) override;
    bool queryColorSupported(const QString &printerName, bool &supported) override;
    bool printFile(const QString &printerName, const QString &filename,
                   const QString &title, int numOptions, cups_option_t *options) override;

    QString defaultPrinterName() const { return m_defaultPrinterName; }

private:
    bool loadSymbols();

    QLibrary m_lib;
    bool m_loaded = false;
    QString m_defaultPrinterName;
    bool m_colorSupported = false;

    void *m_cupsGetDests = nullptr;
    void *m_cupsFreeDests = nullptr;
    void *m_cupsDoRequest = nullptr;
    void *m_ippNewRequest = nullptr;
    void *m_ippAddString = nullptr;
    void *m_ippAddStrings = nullptr;
    void *m_ippFindAttribute = nullptr;
    void *m_cupsPrintFile = nullptr;
    void *m_cupsGetOption = nullptr;
    void *m_ippDelete = nullptr;
    void *m_ippGetBoolean = nullptr;
};

#endif // CUPSCLIENT_H

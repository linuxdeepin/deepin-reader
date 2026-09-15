// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "cupsclient.h"

#include <cups/cups.h>
#include <cups/ipp.h>

#include <QUrl>
#include <QDebug>

#include <cstdlib>

typedef int (*cupsGetDests_fn)(cups_dest_t **dests);
typedef void (*cupsFreeDests_fn)(int num_dests, cups_dest_t *dests);
typedef ipp_t *(*cupsDoRequest_fn)(http_t *http, ipp_t *request, const char *resource);
typedef ipp_t *(*ippNewRequest_fn)(ipp_op_t op);
typedef void (*ippAddString_fn)(ipp_t *ipp, ipp_tag_t group, ipp_tag_t tag, const char *name, const char *language, const char *value);
typedef void (*ippAddStrings_fn)(ipp_t *ipp, ipp_tag_t group, ipp_tag_t tag, const char *name, int num_values, const char *language, const char *const *values);
typedef ipp_attribute_t *(*ippFindAttribute_fn)(ipp_t *ipp, const char *name, ipp_tag_t type);
typedef int (*cupsPrintFile_fn)(const char *printer, const char *filename, const char *title, int num_options, cups_option_t *options);
typedef const char *(*cupsGetOption_fn)(const char *name, int num_options, cups_option_t *options);
typedef void (*ippDelete_fn)(ipp_t *ipp);
typedef int (*ippGetBoolean_fn)(ipp_attribute_t *attr, int element);

CupsClient::CupsClient()
{
}

CupsClient::~CupsClient()
{
}

bool CupsClient::loadSymbols()
{
    m_cupsGetDests = (void *)m_lib.resolve("cupsGetDests");
    m_cupsFreeDests = (void *)m_lib.resolve("cupsFreeDests");
    m_cupsDoRequest = (void *)m_lib.resolve("cupsDoRequest");
    m_ippNewRequest = (void *)m_lib.resolve("ippNewRequest");
    m_ippAddString = (void *)m_lib.resolve("ippAddString");
    m_ippAddStrings = (void *)m_lib.resolve("ippAddStrings");
    m_ippFindAttribute = (void *)m_lib.resolve("ippFindAttribute");
    m_cupsPrintFile = (void *)m_lib.resolve("cupsPrintFile");
    m_cupsGetOption = (void *)m_lib.resolve("cupsGetOption");
    m_ippDelete = (void *)m_lib.resolve("ippDelete");
    m_ippGetBoolean = (void *)m_lib.resolve("ippGetBoolean");

    return m_cupsGetDests && m_cupsFreeDests && m_cupsDoRequest &&
           m_ippNewRequest && m_ippAddString && m_ippAddStrings &&
           m_ippFindAttribute && m_cupsPrintFile && m_cupsGetOption && m_ippDelete && m_ippGetBoolean;
}

bool CupsClient::init()
{
    m_lib.setFileName(QStringLiteral("cups"));
    if (!m_lib.load()) {
        qWarning() << "Failed to dlopen libcups:" << m_lib.errorString();
        return false;
    }

    m_loaded = loadSymbols();
    if (!m_loaded) {
        qWarning() << "Failed to resolve CUPS symbols";
        return false;
    }

    return true;
}

bool CupsClient::checkEnvironment()
{
    if (!m_loaded)
        return false;

    cups_dest_t *dests = nullptr;
    int numDest = ((cupsGetDests_fn)m_cupsGetDests)(&dests);
    if (numDest <= 0) {
        qWarning() << "No printers found";
        return false;
    }

    for (int i = 0; i < numDest; ++i) {
        if (dests[i].is_default) {
            m_defaultPrinterName = QString::fromUtf8(dests[i].name);
            ((cupsFreeDests_fn)m_cupsFreeDests)(numDest, dests);
            return true;
        }
    }

    m_defaultPrinterName = QString::fromUtf8(dests[0].name);
    ((cupsFreeDests_fn)m_cupsFreeDests)(numDest, dests);

    return true;
}

bool CupsClient::isColorSupported()
{
    if (!m_loaded || m_defaultPrinterName.isEmpty())
        return false;

    bool supported = false;
    if (queryColorSupported(m_defaultPrinterName, supported)) {
        m_colorSupported = supported;
    } else {
        m_colorSupported = false;
    }
    return m_colorSupported;
}

bool CupsClient::getCupsDests(cups_dest_t **dests)
{
    if (!m_loaded || !m_cupsGetDests)
        return false;

    int numDest = ((cupsGetDests_fn)m_cupsGetDests)(dests);
    return numDest > 0;
}

bool CupsClient::queryColorSupported(const QString &printerName, bool &supported)
{
    if (!m_loaded)
        return false;

    ipp_t *request = ((ippNewRequest_fn)m_ippNewRequest)(IPP_GET_PRINTER_ATTRIBUTES);
    if (!request)
        return false;

    QString printerUri = QStringLiteral("ipp://localhost/printers/") +
                         QString::fromUtf8(QUrl::toPercentEncoding(printerName));

    ((ippAddString_fn)m_ippAddString)(request, IPP_TAG_OPERATION, IPP_TAG_URI,
                                      "printer-uri", nullptr,
                                      printerUri.toUtf8().constData());

    static const char *requestedAttrs[] = {"color-supported", "printer-type"};
    ((ippAddStrings_fn)m_ippAddStrings)(request, IPP_TAG_OPERATION, IPP_TAG_KEYWORD,
                                        "requested-attributes", 2, nullptr,
                                        requestedAttrs);

    ipp_t *response = ((cupsDoRequest_fn)m_cupsDoRequest)(CUPS_HTTP_DEFAULT, request, "/");
    if (!response) {
        supported = false;
        return false;
    }

    ipp_attribute_t *attr = ((ippFindAttribute_fn)m_ippFindAttribute)(response, "color-supported", IPP_TAG_BOOLEAN);
    if (attr) {
        supported = ((ippGetBoolean_fn)m_ippGetBoolean)(attr, 0);
        ((ippDelete_fn)m_ippDelete)(response);
        return true;
    }

    ((ippDelete_fn)m_ippDelete)(response);
    supported = false;
    return false;
}

bool CupsClient::submitJob(const QString &pdfPath, const QString &title, const PrintSettings &settings)
{
    if (!m_loaded || m_defaultPrinterName.isEmpty())
        return false;

    bool printerSupportsColor = m_colorSupported;
    QStringList optionList = toCupsOptions(settings, printerSupportsColor);

    int numOptions = optionList.size() / 2;
    cups_option_t *options = nullptr;
    if (numOptions > 0) {
        options = (cups_option_t *)calloc(numOptions, sizeof(cups_option_t));
        if (!options)
            return false;

        for (int i = 0; i < numOptions; ++i) {
            options[i].name = strdup(optionList[i * 2].toUtf8().constData());
            options[i].value = strdup(optionList[i * 2 + 1].toUtf8().constData());
        }
    }

    int result = ((cupsPrintFile_fn)m_cupsPrintFile)(
        m_defaultPrinterName.toUtf8().constData(),
        pdfPath.toUtf8().constData(),
        title.toUtf8().constData(),
        numOptions, options);

    if (options) {
        for (int i = 0; i < numOptions; ++i) {
            free(options[i].name);
            free(options[i].value);
        }
        free(options);
    }

    return result > 0;
}

bool CupsClient::printFile(const QString &printerName, const QString &filename,
                           const QString &title, int numOptions, cups_option_t *options)
{
    if (!m_loaded || !m_cupsPrintFile)
        return false;

    int result = ((cupsPrintFile_fn)m_cupsPrintFile)(
        printerName.toUtf8().constData(),
        filename.toUtf8().constData(),
        title.toUtf8().constData(),
        numOptions, options);

    return result > 0;
}

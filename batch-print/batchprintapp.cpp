// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "batchprintapp.h"
#include "formatconverter.h"
#include "notifyclient.h"
#include "errormessages.h"

#include <QTemporaryDir>
#include <QFileInfo>
#include <QDebug>

BatchPrintApp::BatchPrintApp(ICupsApi *cupsApi)
{
    if (cupsApi) {
        m_cupsApi = cupsApi;
        m_ownsCupsApi = false;
    } else {
        m_cupsApi = new CupsClient();
        m_ownsCupsApi = true;
    }
}

BatchPrintApp::~BatchPrintApp()
{
    if (m_ownsCupsApi)
        delete m_cupsApi;
}

int BatchPrintApp::run(const QStringList &fileList)
{
    CupsClient *cups = dynamic_cast<CupsClient *>(m_cupsApi);
    if (cups) {
        if (!cups->init()) {
            qWarning() << "CUPS init failed";
            NotifyClient::notifyError(ErrorMessages::cupsUnavailable());
            return 2;
        }
        if (!cups->checkEnvironment()) {
            qWarning() << "CUPS environment check failed";
            NotifyClient::notifyError(ErrorMessages::noDefaultPrinter());
            return 2;
        }
        if (cups->isColorSupported()) {
            m_settings.colorMode = ColorMode::Auto;
        } else {
            m_settings.colorMode = ColorMode::Gray;
        }
    }

    int total = fileList.size();
    int succeeded = 0;
    QStringList failedFiles;

    for (const QString &filePath : fileList) {
        QTemporaryDir tempDir;
        if (!tempDir.isValid()) {
            failedFiles.append(QFileInfo(filePath).fileName());
            continue;
        }

        QString outputPdfPath;
        QString errorMsg;
        if (!FormatConverter::convertToPdf(filePath, tempDir.path(),
                                           outputPdfPath, errorMsg)) {
            qWarning() << errorMsg;
            failedFiles.append(QFileInfo(filePath).fileName());
            continue;
        }

        QString jobTitle = QFileInfo(filePath).fileName();
        bool printOk = m_cupsApi->submitJob(outputPdfPath, jobTitle, m_settings);

        if (!printOk) {
            failedFiles.append(QFileInfo(filePath).fileName());
            continue;
        }

        ++succeeded;
    }

    NotifyClient::notifyResult(total, succeeded, failedFiles);

    if (succeeded == total)
        return 0;
    return 1;
}

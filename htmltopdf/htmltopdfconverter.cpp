// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "htmltopdfconverter.h"

#include <QApplication>

HtmltoPdfConverter::HtmltoPdfConverter(const QString &inputPath, const QString &outputPath)
    : m_inputPath(inputPath)
    , m_outputPath(outputPath)
    , m_page(new QWebEnginePage)
{
    qDebug() << "HtmltoPdfConverter initialized with input:" << inputPath << "output:" << outputPath;

    connect(m_page.data(), &QWebEnginePage::loadFinished,
            this, &HtmltoPdfConverter::loadFinished);
    connect(m_page.data(), &QWebEnginePage::pdfPrintingFinished,
            this, &HtmltoPdfConverter::pdfPrintingFinished);
}

int HtmltoPdfConverter::run()
{
    qDebug() << "Starting HTML to PDF conversion process";
    QUrl url = QUrl::fromUserInput(m_inputPath);
    qDebug() << "Loading HTML file from path:" << url;
    m_page->load(url);
    return QCoreApplication::exec();
}

void HtmltoPdfConverter::loadFinished(bool ok)
{
    if (!ok) {
        qWarning() << QString("Failed to load URL '%1'").arg(m_inputPath);
        QCoreApplication::exit(1);
    }
    qInfo() << QString("Successfully loaded URL '%1'").arg(m_inputPath);
    m_page->printToPdf(m_outputPath);
}

void HtmltoPdfConverter::pdfPrintingFinished(const QString &filePath, bool success)
{
    if (!success) {
        qWarning() << QString("Failed to print to output file '%1'").arg(filePath);
        QCoreApplication::exit(1);
    } else {
        QCoreApplication::quit();
    }
}

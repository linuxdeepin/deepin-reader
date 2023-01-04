/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     chendu <chendu@uniontech.com>
*
* Maintainer: chendu <chendu@uniontech.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "htmltopdfconverter.h"

#include <QApplication>

HtmltoPdfConverter::HtmltoPdfConverter(const QString &inputPath, const QString &outputPath)
    : m_inputPath(inputPath)
    , m_outputPath(outputPath)
    , m_page(new QWebEnginePage)
{
    connect(m_page.data(), &QWebEnginePage::loadFinished,
            this, &HtmltoPdfConverter::loadFinished);
    connect(m_page.data(), &QWebEnginePage::pdfPrintingFinished,
            this, &HtmltoPdfConverter::pdfPrintingFinished);
}

int HtmltoPdfConverter::run()
{
    QUrl url = QUrl::fromUserInput(m_inputPath);
    qDebug() << "htmltoPdf 加载的文件路径: " <<  url;
    m_page->load(url);
    return QCoreApplication::exec();
}

void HtmltoPdfConverter::loadFinished(bool ok)
{
    if (!ok) {
        qInfo() << QString("failed to load URL '%1'").arg(m_inputPath);
        QCoreApplication::exit(1);
    }
    qInfo() << QString("success to load URL '%1'").arg(m_inputPath);
    m_page->printToPdf(m_outputPath);
}

void HtmltoPdfConverter::pdfPrintingFinished(const QString &filePath, bool success)
{
    if (!success) {
        qInfo() << QString("failed to print to output file '%1'").arg(filePath);
        QCoreApplication::exit(1);
    } else {
        QCoreApplication::quit();
    }
}

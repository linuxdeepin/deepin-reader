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
#include "html2pdfconverter.h"

#include <QApplication>
#include <QWebEnginePage>

Html2PdfConverter::Html2PdfConverter(const QString &inputPath, const QString &outputPath, QObject *parent)
    : QObject(parent)
    , m_inputPath(inputPath)
    , m_outputPath(outputPath)
{

}

void Html2PdfConverter::run()
{
    m_page = new QWebEnginePage(this);
    connect(m_page, &QWebEnginePage::loadFinished, this, &Html2PdfConverter::loadFinished);
    connect(m_page, &QWebEnginePage::pdfPrintingFinished, this, &Html2PdfConverter::pdfPrintingFinished);
    m_page->load(QUrl::fromUserInput(m_inputPath));
}

void Html2PdfConverter::loadFinished(bool ok)
{
    if (!ok) {
        qInfo() << QString("failed to load URL '%1'").arg(m_inputPath);
        Q_EMIT sigQuit();
        return;
    }

    m_page->printToPdf(m_outputPath); //将页面的当前内容呈现为PDF文档，并将其保存在m_outputPath
}

void Html2PdfConverter::pdfPrintingFinished(const QString &filePath, bool success)
{
    if (!success) {
        qInfo() << QString("failed to print to output file '%1'").arg(filePath);
    }
    Q_EMIT sigQuit();
}

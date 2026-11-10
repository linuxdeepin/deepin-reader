// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "htmltopdfconverter.h"

#include <QApplication>
#include <QCommandLineParser>

#include "ddlog.h"

int main(int argc, char *argv[])
{
    qCDebug(htmltopdfLog) << "Starting html2pdf converter application";

    QApplication app(argc, argv);
    QApplication::setApplicationName("html2pdf");
    QApplication::setApplicationVersion("v1.0");

    //需求文案中不涉及，不做翻译
    QCommandLineParser parser;
    parser.setApplicationDescription(
        /*QCoreApplication::translate("main", */("Converts the web page INPUT into the PDF file OUTPUT."));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument(
        /*QCoreApplication::translate("main", */("INPUT"),
        /*QCoreApplication::translate("main", */("Input URL for PDF conversion."));
    parser.addPositionalArgument(
        /*QCoreApplication::translate("main", */("OUTPUT"),
        /*QCoreApplication::translate("main", */("Output file name for PDF conversion."));

    parser.process(QCoreApplication::arguments());

    const QStringList requiredArguments = parser.positionalArguments();
    if (requiredArguments.size() != 2) {
        qCCritical(htmltopdfLog) << "Invalid number of arguments, expected 2 but got" << requiredArguments.size();
        parser.showHelp(1);
    }
    qCInfo(htmltopdfLog) << "Input file:" << requiredArguments.at(0) << "Output PDF:" << requiredArguments.at(1);

    qCInfo(htmltopdfLog) << "Creating PDF converter instance";
    HtmltoPdfConverter converter(requiredArguments.at(0), requiredArguments.at(1));
    return converter.run();
}

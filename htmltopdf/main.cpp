// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "htmltopdfconverter.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QProcessEnvironment>
#include <QDebug>

bool isWayland();

int main(int argc, char *argv[])
{
    qDebug() << "Starting html2pdf converter application";

    QApplication app(argc, argv);
    QApplication::setApplicationName("html2pdf");
    QApplication::setApplicationVersion("v1.0");

    bool wayland = isWayland();
    qDebug() << "Wayland environment detected:" << wayland;
    if (wayland) {
        qDebug() << "Wayland detected, disabling GPU";
        // 解决klu panguV平台使用QWebEnginePage崩溃的问题，不支持gpu渲染
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu");
    }
    // 解决__sw_64__平台使用QWebEnginePage崩溃的问题
#ifdef __sw_64__
    qDebug() << "Running on __sw_64__ platform, --js-flags=--jitless";
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--js-flags=--jitless");
#endif

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
        qCritical() << "Invalid number of arguments, expected 2 but got" << requiredArguments.size();
        parser.showHelp(1);
    }
    qInfo() << "Input file:" << requiredArguments.at(0) << "Output PDF:" << requiredArguments.at(1);

    qInfo() << "Creating PDF converter instance";
    HtmltoPdfConverter converter(requiredArguments.at(0), requiredArguments.at(1));
    return converter.run();
}

bool isWayland()
{
    qDebug() << "Checking display environment";
    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));

    if (XDG_SESSION_TYPE == QLatin1String("wayland") || WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive)) {
        qDebug() << "Wayland session detected";
        return true;
    } else {
        qDebug() << "X11 session detected";
        return false;
    }
}

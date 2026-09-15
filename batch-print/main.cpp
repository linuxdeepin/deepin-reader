// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "batchprintapp.h"

#include <DGuiApplicationHelper>

#include <QGuiApplication>
#include <QCommandLineParser>
#include <QProcessEnvironment>
#include <QFileInfo>
#include <QDebug>

#include <cstdio>

DGUI_USE_NAMESPACE

int main(int argc, char *argv[])
{
    if (!qEnvironmentVariableIsSet("DISPLAY") &&
        !qEnvironmentVariableIsSet("WAYLAND_DISPLAY")) {
        qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    QGuiApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("deepin-reader-batchprint"));

    DGuiApplicationHelper::loadTranslator("deepin-reader", "deepin-reader");

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("Batch print documents silently"));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("files"),
                                 QStringLiteral("Document files to print"),
                                 QStringLiteral("[files...]"));
    parser.process(app);

    QStringList files = parser.positionalArguments();
    if (files.isEmpty()) {
        fprintf(stderr, "No files specified.\n");
        return 2;
    }

    BatchPrintApp batchApp;
    return batchApp.run(files);
}

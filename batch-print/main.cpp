// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
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
    parser.setApplicationDescription(
        QGuiApplication::translate("batchprint", "Batch print documents silently"));
    parser.addHelpOption();
    parser.addPositionalArgument(QStringLiteral("files"),
                                 QGuiApplication::translate("batchprint", "Document files to print"),
                                 QStringLiteral("[files...]"));
    parser.process(app);

    QStringList files = parser.positionalArguments();
    if (files.isEmpty()) {
        fprintf(stderr, "%s\n",
                QGuiApplication::translate("batchprint", "No files specified.")
                    .toUtf8().constData());
        return 2;
    }

    BatchPrintApp batchApp;
    return batchApp.run(files);
}

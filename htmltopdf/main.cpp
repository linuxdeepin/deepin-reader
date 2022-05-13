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
#include <QCommandLineParser>
#include <QProcessEnvironment>

bool isWayland();

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("html2pdf");
    QApplication::setApplicationVersion("v1.0");

    if (isWayland()) {
        // 解决klu panguV平台使用QWebEnginePage崩溃的问题，不支持gpu渲染
        qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--disable-gpu");
    }

    // 解决__sw_64__平台使用QWebEnginePage崩溃的问题
#ifdef __sw_64__
    qputenv("QTWEBENGINE_CHROMIUM_FLAGS", "--no-sandbox");
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
    if (requiredArguments.size() != 2)
        parser.showHelp(1);

    HtmltoPdfConverter converter(requiredArguments.at(0), requiredArguments.at(1));
    return converter.run();
}

bool isWayland()
{
    auto e = QProcessEnvironment::systemEnvironment();
    QString XDG_SESSION_TYPE = e.value(QStringLiteral("XDG_SESSION_TYPE"));
    QString WAYLAND_DISPLAY = e.value(QStringLiteral("WAYLAND_DISPLAY"));

    if (XDG_SESSION_TYPE == QLatin1String("wayland") || WAYLAND_DISPLAY.contains(QLatin1String("wayland"), Qt::CaseInsensitive)) {
        return true;
    } else {
        return false;
    }
}

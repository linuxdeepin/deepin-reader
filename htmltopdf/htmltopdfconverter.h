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
#ifndef HTML2PDFCONVERTER_H
#define HTML2PDFCONVERTER_H

#include <QWebEnginePage>

/**
 * @brief The HtmltoPdfConverter class
 * html文件转换为pdf格式文件功能
 */
class HtmltoPdfConverter : public QObject
{
    Q_OBJECT
public:
    explicit HtmltoPdfConverter(const QString &inputPath, const QString &outputPath);
    int run();

private slots:
    void loadFinished(bool ok);
    void pdfPrintingFinished(const QString &filePath, bool success);

private:
    QString m_inputPath; //输入文件html
    QString m_outputPath; //输出文件pdf
    QScopedPointer<QWebEnginePage> m_page;
};

#endif // HTML2PDFCONVERTER_H

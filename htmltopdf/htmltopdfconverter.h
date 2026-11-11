// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HTML2PDFCONVERTER_H
#define HTML2PDFCONVERTER_H

#include <QObject>

class QTextDocument;

/**
 * @brief The HtmltoPdfConverter class
 * html文件转换为pdf格式文件功能
 * 基于 QTextDocument + QPrinter 实现（Qt 自带，无 QtWebEngine 依赖）。
 * QTextDocument 支持的 HTML 富文本子集（标题/段落/表格/列表/图片/字体/对齐等）
 * 已完全覆盖 pandoc 生成的 HTML 片段。
 */
class HtmltoPdfConverter : public QObject
{
    Q_OBJECT
public:
    explicit HtmltoPdfConverter(const QString &inputPath, const QString &outputPath);
    int run();

private:
    QString m_inputPath; //输入文件html
    QString m_outputPath; //输出文件pdf
};

#endif // HTML2PDFCONVERTER_H

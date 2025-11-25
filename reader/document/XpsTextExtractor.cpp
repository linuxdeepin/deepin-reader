// Copyright (C) 2019 ~ 2025 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XpsTextExtractor.h"

#include "ddlog.h"

#include <QByteArray>
#include <QFile>
#include <QPolygonF>
#include <QXmlStreamReader>

#include <algorithm>

#ifdef signals
#pragma push_macro("signals")
#undef signals
#endif
#ifdef slots
#pragma push_macro("slots")
#undef slots
#endif

extern "C" {
#include <archive.h>
#include <archive_entry.h>
}

#ifdef slots
#pragma pop_macro("slots")
#endif
#ifdef signals
#pragma pop_macro("signals")
#endif

namespace deepin_reader {

QList<Word> XpsTextExtractor::extractWords(const QString &filePath, int pageIndex)
{
    QList<TextRun> textRuns = extractTextRuns(filePath, pageIndex);
    if (textRuns.isEmpty()) {
        return {};
    }

    // 将TextRun转换为Word列表
    // 为了支持字符级选择，需要将每个Glyph的文本拆分成单个字符
    QList<Word> allWords;
    for (const TextRun &run : textRuns) {
        if (!run.glyphs.isEmpty()) {
            // 使用glyph信息，但将文本拆分成单个字符
            for (const GlyphInfo &glyph : run.glyphs) {
                if (glyph.text.isEmpty() || glyph.boundingBox.isEmpty()) {
                    continue;
                }

                // 计算每个字符的边界框
                qreal currentX = glyph.position.x();
                qreal baseY = glyph.position.y();
                // 改进字符高度计算：考虑字体的ascent和descent
                // 通常ascent约占字体大小的70-80%，descent约占20-30%
                // 使用更合理的估算：ascent = fontSize * 0.75, descent = fontSize * 0.25
                qreal charAscent = glyph.fontSize * 0.75;
                qreal charDescent = glyph.fontSize * 0.25;
                qreal charHeight = charAscent + charDescent;

                for (int i = 0; i < glyph.text.length(); ++i) {
                    QChar ch = glyph.text.at(i);
                    // 优先使用Indices中的精确宽度，否则使用估算
                    double charWidth;
                    if (i < glyph.charWidths.size() && glyph.charWidths[i] > 0) {
                        charWidth = glyph.charWidths[i];
                    } else {
                        charWidth = estimateCharWidth(ch, glyph.fontSize);
                    }

                    // 创建单个字符的边界框
                    // OriginY是基线位置，所以字符顶部 = baseY - charAscent
                    QRectF charBaseRect(currentX, baseY - charAscent, charWidth, charHeight);

                    // 应用变换矩阵
                    QPolygonF charBasePolygon;
                    charBasePolygon << charBaseRect.topLeft() << charBaseRect.topRight()
                                    << charBaseRect.bottomRight() << charBaseRect.bottomLeft();
                    QPolygonF transformedPolygon = glyph.transform.map(charBasePolygon);
                    QRectF charBoundingBox = transformedPolygon.boundingRect();

                    // 创建Word对象
                    Word word;
                    word.text = QString(ch);
                    word.boundingBox = charBoundingBox;
                    allWords.append(word);

                    // 移动到下一个字符位置
                    currentX += charWidth;
                }
            }
        } else if (!run.text.isEmpty() && !run.boundingBox.isEmpty()) {
            // 如果没有glyph信息，将TextRun的文本拆分成单个字符
            // 估算每个字符的宽度
            qreal avgCharWidth = run.boundingBox.width() / run.text.length();
            qreal currentX = run.boundingBox.x();

            for (int i = 0; i < run.text.length(); ++i) {
                QChar ch = run.text.at(i);
                Word word;
                word.text = QString(ch);
                word.boundingBox = QRectF(currentX, run.boundingBox.y(), avgCharWidth, run.boundingBox.height());
                allWords.append(word);
                currentX += avgCharWidth;
            }
        }
    }

    if (allWords.isEmpty()) {
        return {};
    }

    // 按位置排序（从上到下，从左到右）
    std::sort(allWords.begin(), allWords.end(), [](const Word &a, const Word &b) {
        // 首先按Y坐标排序（从上到下）
        const double yThreshold = 5.0; // Y坐标容差
        if (qAbs(a.boundingBox.y() - b.boundingBox.y()) > yThreshold) {
            return a.boundingBox.y() < b.boundingBox.y();
        }
        // Y坐标相近时，按X坐标排序（从左到右）
        return a.boundingBox.x() < b.boundingBox.x();
    });

    // 对于字符级选择，保持每个字符的精确边界框，不需要统一行高
    // 直接返回排序后的字符列表
    return allWords;
}

QList<XpsTextExtractor::TextRun> XpsTextExtractor::extractTextRuns(const QString &filePath, int pageIndex)
{
    QByteArray xmlData = readFixedPageFromZip(filePath, pageIndex);
    if (xmlData.isEmpty()) {
        return {};
    }

    return parseFixedPage(xmlData, pageIndex);
}

QByteArray XpsTextExtractor::readFixedPageFromZip(const QString &filePath, int pageIndex)
{
    QString fixedPagePath = findFixedPagePath(filePath, pageIndex);
    if (fixedPagePath.isEmpty()) {
        return QByteArray();
    }

    struct archive *a = archive_read_new();
    struct archive_entry *entry = nullptr;

    if (!a) {
        return QByteArray();
    }

    archive_read_support_format_zip(a);
    archive_read_support_format_all(a);

    QByteArray utf8Path = QFile::encodeName(filePath);
    int r = archive_read_open_filename(a, utf8Path.constData(), 10240);
    if (r != ARCHIVE_OK) {
        archive_read_free(a);
        return QByteArray();
    }

    QByteArray result;
    bool found = false;

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        QString entryPath = QString::fromUtf8(archive_entry_pathname(entry));
            if (entryPath == fixedPagePath) {

            la_int64_t size = archive_entry_size(entry);
            if (size > 0 && size < 100 * 1024 * 1024) { // 限制100MB
                result.resize(static_cast<int>(size));
                la_ssize_t readSize = archive_read_data(a, result.data(), size);
                if (readSize == size) {
                    found = true;
                } else {
                    // 读取失败，清理并返回
                    archive_read_free(a);
                    return QByteArray();
                }
            }
            break;
        }
    }

    archive_read_free(a);

    if (!found) {
        return QByteArray();
    }

    return result;
}

QString XpsTextExtractor::findFixedPagePath(const QString &filePath, int pageIndex)
{
    Q_UNUSED(filePath) // 当前实现使用标准路径，未来可以解析[Content_Types].xml

    if (pageIndex < 0) {
        return QString();
    }

    // XPS标准格式: Documents/1/Pages/{pageNumber}.fpage
    // pageIndex从0开始，但XPS页面编号通常从1开始
    int pageNumber = pageIndex + 1;
    return QStringLiteral("Documents/1/Pages/%1.fpage").arg(pageNumber);
}

QList<XpsTextExtractor::TextRun> XpsTextExtractor::parseFixedPage(const QByteArray &xmlData, int pageIndex)
{
    Q_UNUSED(pageIndex);
    QList<TextRun> textRuns;
    QXmlStreamReader xml(xmlData);
    QList<QTransform> transformStack;
    transformStack.append(QTransform()); // 初始单位矩阵
    
    // 用于跟踪当前是否在RenderTransform子元素内部
    int renderTransformDepth = 0;

    while (!xml.atEnd() && !xml.hasError()) {
        QXmlStreamReader::TokenType token = xml.readNext();

        if (token == QXmlStreamReader::StartElement) {
            QString elementName = xml.name().toString();

            if (elementName == QLatin1String("FixedPage")) {
                // 检查RenderTransform属性
                QString transformStr = xml.attributes().value(QLatin1String("RenderTransform")).toString();
                if (!transformStr.isEmpty()) {
                    QTransform transform = parseTransformMatrix(transformStr);
                    transformStack.last() = transformStack.last() * transform;
                }
            } else if (elementName == QLatin1String("Canvas") || elementName == QLatin1String("Path")) {
                // 处理嵌套变换
                QString transformStr = xml.attributes().value(QLatin1String("RenderTransform")).toString();
                if (!transformStr.isEmpty()) {
                    // 属性形式的RenderTransform
                    QTransform transform = parseTransformMatrix(transformStr);
                    transformStack.append(transformStack.last() * transform);
                } else {
                    // 没有属性形式的RenderTransform，可能需要查找子元素形式
                    transformStack.append(transformStack.last());
                }
            } else if (elementName == QLatin1String("Canvas.RenderTransform") || 
                       elementName == QLatin1String("Path.RenderTransform")) {
                // 进入RenderTransform子元素
                renderTransformDepth++;
            } else if (elementName == QLatin1String("MatrixTransform")) {
                // 检查是否在Canvas.RenderTransform或Path.RenderTransform内部
                // 如果是，解析Matrix属性并应用到当前transformStack
                if (renderTransformDepth > 0 && !transformStack.isEmpty()) {
                    QString matrixStr = xml.attributes().value(QLatin1String("Matrix")).toString();
                    if (!matrixStr.isEmpty()) {
                        QTransform transform = parseTransformMatrix(matrixStr);
                        transformStack.last() = transformStack.last() * transform;
                    }
                }
            } else if (elementName == QLatin1String("Glyphs")) {
                QTransform parentTransform = transformStack.isEmpty() ? QTransform() : transformStack.last();
                GlyphInfo glyph = parseGlyphs(xml, parentTransform);
                if (!glyph.text.isEmpty()) {
                    TextRun run;
                    run.text = glyph.text;
                    run.boundingBox = glyph.boundingBox;
                    run.glyphs.append(glyph);
                    textRuns.append(run);
                }
            }
        } else if (token == QXmlStreamReader::EndElement) {
            QString elementName = xml.name().toString();
            if (elementName == QLatin1String("Canvas") || elementName == QLatin1String("Path")) {
                if (transformStack.size() > 1) {
                    transformStack.removeLast();
                }
            } else if (elementName == QLatin1String("Canvas.RenderTransform") || 
                       elementName == QLatin1String("Path.RenderTransform")) {
                // RenderTransform子元素结束
                if (renderTransformDepth > 0) {
                    renderTransformDepth--;
                }
            }
        }
    }

    if (xml.hasError()) {
        return {};
    }

    return textRuns;
}

XpsTextExtractor::GlyphInfo XpsTextExtractor::parseGlyphs(QXmlStreamReader &xml, const QTransform &parentTransform)
{
    GlyphInfo glyph;
    glyph.transform = parentTransform;
    glyph.fontSize = 12.0; // 默认字体大小

    QXmlStreamAttributes attrs = xml.attributes();

    // 读取UnicodeString（文本内容）
    QString unicodeString = attrs.value(QLatin1String("UnicodeString")).toString();
    if (unicodeString.isEmpty()) {
        // 跳过空文本的Glyphs
        return glyph;
    }
    glyph.text = unicodeString;

    // 读取位置
    bool okX = false, okY = false;
    double originX = attrs.value(QLatin1String("OriginX")).toDouble(&okX);
    double originY = attrs.value(QLatin1String("OriginY")).toDouble(&okY);
    if (!okX || !okY) {
        return glyph;
    }
    glyph.position = QPointF(originX, originY);

    // 读取字体大小
    bool okSize = false;
    double fontSize = attrs.value(QLatin1String("FontRenderingEmSize")).toDouble(&okSize);
    if (!okSize || fontSize <= 0) {
        fontSize = 12.0; // 默认值
    }
    glyph.fontSize = fontSize;

    // 读取字体URI
    glyph.fontUri = attrs.value(QLatin1String("FontUri")).toString();

    // 读取Indices属性（字符级位置信息）
    QString indicesStr = attrs.value(QLatin1String("Indices")).toString();
    if (!indicesStr.isEmpty()) {
        // 解析Indices获取精确的字符宽度
        glyph.charWidths = parseIndices(indicesStr, unicodeString.length(), fontSize);
    } else {
        glyph.charWidths.clear();
    }

    // 读取RenderTransform（如果有）
    QString transformStr = attrs.value(QLatin1String("RenderTransform")).toString();
    if (!transformStr.isEmpty()) {
        QTransform localTransform = parseTransformMatrix(transformStr);
        glyph.transform = parentTransform * localTransform;
    }

    // 计算边界框
    glyph.boundingBox = calculateGlyphBoundingBox(glyph);

    return glyph;
}

QRectF XpsTextExtractor::calculateGlyphBoundingBox(const GlyphInfo &glyph)
{
    if (glyph.text.isEmpty()) {
        return QRectF();
    }

    // 估算文本宽度
    double totalWidth = 0.0;
    // 优先使用Indices中的精确宽度
    if (!glyph.charWidths.isEmpty() && glyph.charWidths.size() == glyph.text.length()) {
        for (int i = 0; i < glyph.charWidths.size(); ++i) {
            totalWidth += glyph.charWidths[i];
        }
    } else {
        // 如果没有Indices，使用估算
        for (int i = 0; i < glyph.text.length(); ++i) {
            totalWidth += estimateCharWidth(glyph.text.at(i), glyph.fontSize);
        }
    }

    // 改进文本高度计算：考虑字体的ascent和descent
    // 通常ascent约占字体大小的70-80%，descent约占20-30%
    double charAscent = glyph.fontSize * 0.75;
    double charDescent = glyph.fontSize * 0.25;
    double height = charAscent + charDescent;

    // 创建基础边界框（在文本位置）
    // 注意：XPS坐标系中，OriginY是文本基线位置，所以字符顶部 = OriginY - charAscent
    QRectF baseRect(glyph.position.x(), glyph.position.y() - charAscent, totalWidth, height);

    // 应用变换矩阵
    QPolygonF basePolygon;
    basePolygon << baseRect.topLeft() << baseRect.topRight() 
                << baseRect.bottomRight() << baseRect.bottomLeft();
    QPolygonF transformedPolygon = glyph.transform.map(basePolygon);
    QRectF transformedRect = transformedPolygon.boundingRect();

    return transformedRect;
}

QTransform XpsTextExtractor::parseTransformMatrix(const QString &transformStr)
{
    if (transformStr.isEmpty()) {
        return QTransform();
    }

    // XPS变换矩阵格式: "m11,m12,m21,m22,dx,dy"
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QStringList parts = transformStr.split(QLatin1Char(','), QString::SkipEmptyParts);
    #else
    QStringList parts = transformStr.split(QLatin1Char(','), Qt::SkipEmptyParts);
    #endif
    if (parts.size() != 6) {
        return QTransform();
    }

    bool ok = false;
    double m11 = parts[0].toDouble(&ok);
    if (!ok) return QTransform();
    double m12 = parts[1].toDouble(&ok);
    if (!ok) return QTransform();
    double m21 = parts[2].toDouble(&ok);
    if (!ok) return QTransform();
    double m22 = parts[3].toDouble(&ok);
    if (!ok) return QTransform();
    double dx = parts[4].toDouble(&ok);
    if (!ok) return QTransform();
    double dy = parts[5].toDouble(&ok);
    if (!ok) return QTransform();

    return QTransform(m11, m12, m21, m22, dx, dy);
}

double XpsTextExtractor::estimateCharWidth(QChar ch, double fontSize)
{
    if (fontSize <= 0) {
        return 12.0; // 默认宽度
    }

    // 简单的字符宽度估算
    // 中文字符、日文、韩文等全角字符
    if (ch.unicode() >= 0x4E00 && ch.unicode() <= 0x9FFF) { // CJK统一汉字
        return fontSize;
    }
    if (ch.unicode() >= 0x3040 && ch.unicode() <= 0x309F) { // 平假名
        return fontSize;
    }
    if (ch.unicode() >= 0x30A0 && ch.unicode() <= 0x30FF) { // 片假名
        return fontSize;
    }
    if (ch.unicode() >= 0xAC00 && ch.unicode() <= 0xD7AF) { // 韩文
        return fontSize;
    }

    // 英文字母和数字
    if ((ch >= QLatin1Char('A') && ch <= QLatin1Char('Z')) ||
        (ch >= QLatin1Char('a') && ch <= QLatin1Char('z'))) {
        return fontSize * 0.6;
    }
    if (ch >= QLatin1Char('0') && ch <= QLatin1Char('9')) {
        return fontSize * 0.5;
    }

    // 空格
    if (ch == QLatin1Char(' ') || ch == QChar(0x00A0)) { // 普通空格或非断行空格
        return fontSize * 0.3;
    }

    // 标点符号等，使用中等宽度
    if (ch.isPunct()) {
        return fontSize * 0.4;
    }

    // 其他字符，使用默认值
    return fontSize * 0.6;
}

QList<double> XpsTextExtractor::parseIndices(const QString &indicesStr, int textLength, double fontSize)
{
    QList<double> widths;
    if (indicesStr.isEmpty() || textLength <= 0) {
        return widths;
    }

    // XPS Indices格式：每个字符对应一个值，用分号分隔
    // 格式可能是：",100;,100;,98.864;" 或 "100;100;98.864;"
    // 值表示字符的advance width（相对于字体大小的比例，单位是1/100 em）
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QStringList parts = indicesStr.split(QLatin1Char(';'), QString::SkipEmptyParts);
    #else
    QStringList parts = indicesStr.split(QLatin1Char(';'), Qt::SkipEmptyParts);
    #endif
    
    for (int i = 0; i < parts.size() && i < textLength; ++i) {
        QString part = parts[i].trimmed();
        // 移除可能的逗号前缀
        if (part.startsWith(QLatin1Char(','))) {
            part = part.mid(1);
        }

        bool ok = false;
        double value = part.toDouble(&ok);
        if (ok && value > 0) {
            // Indices中的值是相对于字体大小的比例（单位是1/100 em）
            // 所以实际宽度 = fontSize * value / 100.0
            double width = fontSize * value / 100.0;
            widths.append(width);
        } else {
            // 如果解析失败，跳过（让调用者使用估算值）
        }
    }
    
    return widths;
}

} // namespace deepin_reader


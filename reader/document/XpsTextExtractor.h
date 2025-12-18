// Copyright (C) 2019 ~ 2025 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef XPSTEXTEXTRACTOR_H
#define XPSTEXTEXTRACTOR_H

#include "Model.h"

#include <QList>
#include <QPointF>
#include <QRectF>
#include <QString>
#include <QTransform>

class QXmlStreamReader;

namespace deepin_reader {

/**
 * @brief XPS文本提取器
 * 从XPS文件中提取文本和坐标信息
 */
class XpsTextExtractor
{
public:
    /**
     * @brief Glyph信息结构
     */
    struct GlyphInfo {
        QString text;           // 字符文本
        QPointF position;       // 基础位置 (OriginX, OriginY)
        QRectF boundingBox;    // 字符边界框
        double fontSize;        // 字体大小（点）
        QString fontUri;        // 字体资源URI（相对路径）
        QTransform transform;   // 应用的变换矩阵
        QList<double> charWidths; // 字符宽度列表（从Indices解析，如果可用）
    };

    /**
     * @brief 文本运行结构
     */
    struct TextRun {
        QString text;           // 完整文本
        QRectF boundingBox;    // 文本运行边界框
        QList<GlyphInfo> glyphs; // 字符列表
    };

    /**
     * @brief 从XPS文件路径提取指定页面的文本
     * @param filePath XPS文件路径
     * @param pageIndex 页面索引（从0开始）
     * @return Word列表
     */
    static QList<Word> extractWords(const QString &filePath, int pageIndex);

    /**
     * @brief 从XPS文件路径提取指定页面的文本（返回详细信息）
     * @param filePath XPS文件路径
     * @param pageIndex 页面索引（从0开始）
     * @return TextRun列表
     */
    static QList<TextRun> extractTextRuns(const QString &filePath, int pageIndex);

private:
    /**
     * @brief 解析FixedPage XML文件
     * @param xmlData XML数据
     * @param pageIndex 页面索引（用于日志）
     * @return TextRun列表
     */
    static QList<TextRun> parseFixedPage(const QByteArray &xmlData, int pageIndex);

    /**
     * @brief 解析Glyphs元素
     * @param xml XML读取器（当前位置应在Glyphs元素）
     * @param parentTransform 父变换矩阵
     * @return GlyphInfo结构
     */
    static GlyphInfo parseGlyphs(QXmlStreamReader &xml, const QTransform &parentTransform);

    /**
     * @brief 计算字符边界框
     * @param glyph 字符信息
     * @return 边界框
     */
    static QRectF calculateGlyphBoundingBox(const GlyphInfo &glyph);

    /**
     * @brief 解析变换矩阵字符串 "m11,m12,m21,m22,dx,dy"
     * @param transformStr 变换矩阵字符串
     * @return QTransform对象
     */
    static QTransform parseTransformMatrix(const QString &transformStr);

    /**
     * @brief 估算字符宽度（当字体不可用时）
     * @param ch 字符
     * @param fontSize 字体大小
     * @return 估算的字符宽度
     */
    static double estimateCharWidth(QChar ch, double fontSize);

    /**
     * @brief 解析XPS Indices属性，获取字符宽度列表
     * @param indicesStr Indices属性字符串
     * @param textLength 文本长度
     * @param fontSize 字体大小
     * @return 字符宽度列表（单位：点）
     */
    static QList<double> parseIndices(const QString &indicesStr, int textLength, double fontSize);

    /**
     * @brief 从XPS ZIP包中读取FixedPage文件
     * @param filePath XPS文件路径
     * @param pageIndex 页面索引
     * @return XML数据，失败返回空QByteArray
     */
    static QByteArray readFixedPageFromZip(const QString &filePath, int pageIndex);

    /**
     * @brief 查找FixedPage文件路径
     * @param filePath XPS文件路径
     * @param pageIndex 页面索引
     * @return FixedPage文件在ZIP中的路径，失败返回空字符串
     */
    static QString findFixedPagePath(const QString &filePath, int pageIndex);

    /**
     * @brief 从ZIP文件读取字体文件
     * @param filePath XPS文件路径
     * @param fontUri 字体URI（相对路径）
     * @return 字体文件数据
     */
    static QByteArray readFontFromZip(const QString &filePath, const QString &fontUri);
};

} // namespace deepin_reader

#endif // XPSTEXTEXTRACTOR_H


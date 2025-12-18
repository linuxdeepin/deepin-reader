// Copyright (C) 2019 ~ 2025 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XpsTextExtractor.h"

#include "ddlog.h"

#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QPolygonF>
#include <QXmlStreamReader>
#include <QCache>
#include <QMutex>
#include <QMutexLocker>

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
#include <ft2build.h>
#include FT_FREETYPE_H
#include <cairo-ft.h>
#include <cairo.h>
#include <archive_entry.h>
}

#ifdef slots
#pragma pop_macro("slots")
#endif
#ifdef signals
#pragma pop_macro("signals")
#endif

namespace {

// FreeType 库单例（线程安全）
static FT_Library ftLibrary = nullptr;
static QMutex ftLibraryMutex;
static bool ftLibraryInitialized = false;

// 字体缓存：key = filePath + fontUri，value = cairo_font_face_t*
struct FontCacheKey {
    QString filePath;
    QString fontUri;
    bool operator==(const FontCacheKey &other) const {
        return filePath == other.filePath && fontUri == other.fontUri;
    }
};

uint qHash(const FontCacheKey &key, uint seed = 0) {
    return qHash(key.filePath, seed) ^ qHash(key.fontUri, seed);
}

// 字体数据包装器，用于保持字体数据在内存中
struct FontDataWrapper {
    QByteArray fontData;
    cairo_font_face_t* fontFace;

    FontDataWrapper(const QByteArray &data, cairo_font_face_t* face)
        : fontData(data), fontFace(face) {
        // cairo_ft_font_face_create_for_ft_face 创建的 fontFace 初始引用计数为 1
        // FontDataWrapper 拥有这个引用，析构时释放
    }

    ~FontDataWrapper() {
        if (fontFace) {
            cairo_font_face_destroy(fontFace);
        }
    }
};

static QCache<FontCacheKey, FontDataWrapper> fontCache(100); // 缓存最多100个字体
static QMutex fontCacheMutex;

// 初始化 FreeType 库
void initFreeTypeLibrary() {
    QMutexLocker locker(&ftLibraryMutex);
    if (!ftLibraryInitialized) {
        FT_Error error = FT_Init_FreeType(&ftLibrary);
        if (error == 0) {
            ftLibraryInitialized = true;
        }
    }
}

// 解析 GUID 字符串（用于混淆字体）
bool parseGuid(const QString &str, unsigned short guid[16]) {
    // GUID 格式：XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX (36 字符)
    // 映射字节到 GUID 字符串的位置（基于 libgxps 的实现）
    static const int indexes[] = {6, 4, 2, 0, 11, 9, 16, 14, 19, 21, 24, 26, 28, 30, 32, 34};

    if (str.length() < 36) {
        return false;
    }

    for (int i = 0; i < 16; i++) {
        if (indexes[i] + 1 >= str.length()) {
            return false;
        }
        QChar c1 = str[indexes[i]];
        QChar c2 = str[indexes[i] + 1];

        bool ok1 = false, ok2 = false;
        unsigned hex1 = c1.toLatin1();
        unsigned hex2 = c2.toLatin1();

        if (hex1 >= '0' && hex1 <= '9') {
            hex1 -= '0';
            ok1 = true;
        } else if (hex1 >= 'a' && hex1 <= 'f') {
            hex1 = hex1 - 'a' + 10;
            ok1 = true;
        } else if (hex1 >= 'A' && hex1 <= 'F') {
            hex1 = hex1 - 'A' + 10;
            ok1 = true;
        }

        if (hex2 >= '0' && hex2 <= '9') {
            hex2 -= '0';
            ok2 = true;
        } else if (hex2 >= 'a' && hex2 <= 'f') {
            hex2 = hex2 - 'a' + 10;
            ok2 = true;
        } else if (hex2 >= 'A' && hex2 <= 'F') {
            hex2 = hex2 - 'A' + 10;
            ok2 = true;
        }

        if (!ok1 || !ok2) {
            return false;
        }

        guid[i] = static_cast<unsigned short>(hex1 * 16 + hex2);
    }

    return true;
}

// 处理混淆的字体文件（根据文件名 GUID 进行 XOR 解密）
void deobfuscateFontData(QByteArray &fontData, const QString &fontUri) {
    QString baseName = QFileInfo(fontUri).baseName();
    unsigned short guid[16];

    if (!parseGuid(baseName, guid)) {
        return; // 不是 GUID 格式，不需要解密
    }

    if (fontData.size() < 32) {
        return; // 字体文件太小
    }

    // Obfuscation: XOR font binary with bytes from guid (font's filename)
    static const int mapping[] = {15, 14, 13, 12, 11, 10, 9, 8, 6, 7, 4, 5, 0, 1, 2, 3};

    for (int i = 0; i < 16; i++) {
        fontData[i] = static_cast<char>(static_cast<unsigned char>(fontData[i]) ^ guid[mapping[i]]);
        if (i + 16 < fontData.size()) {
            fontData[i + 16] = static_cast<char>(static_cast<unsigned char>(fontData[i + 16]) ^ guid[mapping[i]]);
        }
    }
}

// FreeType Cairo key（用于设置用户数据）
static cairo_user_data_key_t ft_cairo_key;

// 从ZIP文件读取字体文件（匿名命名空间中的独立实现）
QByteArray readFontFromZipLocal(const QString &filePath, const QString &fontUri) {
    if (filePath.isEmpty() || fontUri.isEmpty()) {
        return QByteArray();
    }

    QString cleanFontUri = fontUri;
    while (cleanFontUri.startsWith(QLatin1String("/")) || cleanFontUri.startsWith(QLatin1String("../"))) {
        if (cleanFontUri.startsWith(QLatin1String("/"))) {
            cleanFontUri = cleanFontUri.mid(1);
        } else if (cleanFontUri.startsWith(QLatin1String("../"))) {
            cleanFontUri = cleanFontUri.mid(3);
        }
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

        if (entryPath == fontUri || entryPath == cleanFontUri ||
            entryPath.endsWith(QLatin1String("/") + cleanFontUri) ||
            entryPath.contains(cleanFontUri.split(QLatin1String("/")).last())) {
            la_int64_t size = archive_entry_size(entry);
            if (size > 0 && size < 50 * 1024 * 1024) {
                result.resize(static_cast<int>(size));
                la_ssize_t readSize = archive_read_data(a, result.data(), size);
                if (readSize == size) {
                    found = true;
                } else {
                    result.clear();
                }
            }
            break;
        }
        archive_read_data_skip(a);
    }

    archive_read_free(a);

    if (!found) {
        return QByteArray();
    }

    return result;
}

// 从字体文件加载 cairo_font_face_t（带缓存）
cairo_font_face_t* loadFontFace(const QString &filePath, const QString &fontUri) {
    FontCacheKey key{filePath, fontUri};

    {
        QMutexLocker locker(&fontCacheMutex);
        FontDataWrapper* cached = fontCache.object(key);
        if (cached) {
            // 返回缓存的字体（FontDataWrapper 持有引用，确保 fontFace 有效）
            return cached->fontFace;
        }
    }

    // 读取字体文件
    QByteArray fontData = readFontFromZipLocal(filePath, fontUri);
    if (fontData.isEmpty()) {
        return nullptr;
    }

    // 处理混淆字体（直接在原数据上操作）
    deobfuscateFontData(fontData, fontUri);

    // 初始化 FreeType
    initFreeTypeLibrary();
    if (!ftLibraryInitialized) {
        return nullptr;
    }

    // 加载字体到 FreeType
    FT_Face face = nullptr;
    FT_Error error = FT_New_Memory_Face(ftLibrary,
                                         reinterpret_cast<const FT_Byte*>(fontData.constData()),
                                         static_cast<FT_Long>(fontData.size()),
                                         0,
                                         &face);
    if (error != 0) {
        return nullptr;
    }

    // 创建 Cairo 字体
    cairo_font_face_t* fontFace = cairo_ft_font_face_create_for_ft_face(face, 0);
    if (cairo_font_face_status(fontFace) != CAIRO_STATUS_SUCCESS) {
        FT_Done_Face(face);
        return nullptr;
    }

    // 设置用户数据，以便在销毁时清理 FreeType face
    if (cairo_font_face_set_user_data(fontFace,
                                       &ft_cairo_key,
                                       face,
                                       reinterpret_cast<cairo_destroy_func_t>(FT_Done_Face)) != CAIRO_STATUS_SUCCESS) {
        cairo_font_face_destroy(fontFace);
        FT_Done_Face(face);
        return nullptr;
    }

    // 创建包装器并缓存（保持 fontData 在内存中）
    {
        QMutexLocker locker(&fontCacheMutex);
        FontDataWrapper* wrapper = new FontDataWrapper(fontData, fontFace);
        fontCache.insert(key, wrapper); // QCache 会管理 wrapper 的生命周期
    }

    return fontFace;
}

// 从字体文件获取字符宽度（使用 FreeType + Cairo）
double getCharWidthFromFont(const QString &filePath, const QString &fontUri,
                            QChar ch, double fontSize) {
    cairo_font_face_t* fontFace = loadFontFace(filePath, fontUri);
    if (!fontFace) {
        return -1.0; // 加载失败，返回 -1 表示需要使用估算值
    }

    // 创建字体矩阵（与 libgxps 一致）
    cairo_matrix_t fontMatrix;
    cairo_matrix_init_identity(&fontMatrix);
    cairo_matrix_scale(&fontMatrix, fontSize, fontSize);

    // 创建 CTM（单位矩阵）
    cairo_matrix_t ctm;
    cairo_matrix_init_identity(&ctm);

    // 创建字体选项
    cairo_font_options_t* fontOptions = cairo_font_options_create();
    cairo_font_options_set_hint_metrics(fontOptions, CAIRO_HINT_METRICS_OFF);

    // 创建 scaled font
    cairo_scaled_font_t* scaledFont = cairo_scaled_font_create(fontFace,
                                                                 &fontMatrix,
                                                                 &ctm,
                                                                 fontOptions);
    cairo_font_options_destroy(fontOptions);

    cairo_status_t scaledFontStatus = cairo_scaled_font_status(scaledFont);
    if (scaledFontStatus != CAIRO_STATUS_SUCCESS) {
        cairo_scaled_font_destroy(scaledFont);
        return -1.0;
    }

    // 将 Unicode 字符转换为 UTF-8（参考 libgxps 的 glyphs_lookup_index）
    QString utf8Str = QString(ch);
    QByteArray utf8Bytes = utf8Str.toUtf8();
    const char* utf8 = utf8Bytes.constData();

    if (utf8Bytes.isEmpty() || *utf8 == '\0') {
        cairo_scaled_font_destroy(scaledFont);
        return -1.0;
    }

    // 使用 cairo_scaled_font_text_to_glyphs 查找 glyph index（参考 libgxps）
    cairo_glyph_t* glyphs = nullptr;
    int numGlyphs = 0;

    cairo_status_t status = cairo_scaled_font_text_to_glyphs(scaledFont,
                                                              0.0, 0.0,
                                                              utf8,
                                                              utf8Bytes.length(),
                                                              &glyphs,
                                                              &numGlyphs,
                                                              nullptr, // clusters
                                                              nullptr, // num_clusters
                                                              nullptr); // cluster_flags

    double width = -1.0;
    if (status == CAIRO_STATUS_SUCCESS && numGlyphs > 0 && glyphs) {
        // 获取 glyph extents（参考 libgxps 的 cairo_scaled_font_glyph_extents 使用方式）
        cairo_text_extents_t extents;
        cairo_scaled_font_glyph_extents(scaledFont, glyphs, numGlyphs, &extents);
        width = extents.x_advance; // 使用 x_advance 作为字符宽度（与 libgxps 一致）
        cairo_glyph_free(glyphs);
    }

    cairo_scaled_font_destroy(scaledFont);

    return width;
}

} // anonymous namespace

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
                qreal charAscent = glyph.fontSize * 0.75;
                qreal charDescent = glyph.fontSize * 0.25;
                qreal charHeight = charAscent + charDescent;

                for (int i = 0; i < glyph.text.length(); ++i) {
                    QChar ch = glyph.text.at(i);
                    double charWidth;
                    if (i < glyph.charWidths.size() && glyph.charWidths[i] >= 0) {
                        charWidth = glyph.charWidths[i];
                    } else {
                        // 尝试从字体文件获取实际宽度
                        double actualWidth = getCharWidthFromFont(filePath, glyph.fontUri, ch, glyph.fontSize);
                        if (actualWidth >= 0) {
                            charWidth = actualWidth;
                        } else {
                            // 如果获取失败，使用估算值
                            charWidth = estimateCharWidth(ch, glyph.fontSize);
                        }
                    }

                    // 创建单个字符的边界框
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
                    transformStack.last() = transform * transformStack.last();
                }
            } else if (elementName == QLatin1String("Canvas") || elementName == QLatin1String("Path")) {
                // 处理嵌套变换
                QString transformStr = xml.attributes().value(QLatin1String("RenderTransform")).toString();
                if (!transformStr.isEmpty()) {
                    // 属性形式的RenderTransform
                    QTransform transform = parseTransformMatrix(transformStr);
                    transformStack.append(transform * transformStack.last());
                } else {
                    // 没有属性形式的RenderTransform，可能需要查找子元素形式
                    transformStack.append(transformStack.last());
                }
            } else if (elementName == QLatin1String("Canvas.RenderTransform") || 
                       elementName == QLatin1String("Path.RenderTransform")) {
                // 进入RenderTransform子元素（Canvas和Path）
                renderTransformDepth++;
            } else if (elementName == QLatin1String("MatrixTransform")) {
                // 检查是否在RenderTransform子元素内部
                if (renderTransformDepth > 0 && !transformStack.isEmpty()) {
                    QString matrixStr = xml.attributes().value(QLatin1String("Matrix")).toString();
                    if (!matrixStr.isEmpty()) {
                        QTransform transform = parseTransformMatrix(matrixStr);
                        transformStack.last() = transform * transformStack.last();
                    }
                }
            } else if (elementName == QLatin1String("Glyphs")) {
                // 解析Glyphs元素
                // 根据libgxps，Glyphs支持两种RenderTransform形式：
                // 1. 属性形式：RenderTransform="..."
                // 2. 子元素形式：<Glyphs.RenderTransform><MatrixTransform Matrix="..."/></Glyphs.RenderTransform>
                QTransform parentTransform = transformStack.isEmpty() ? QTransform() : transformStack.last();

                // 先读取Glyphs的属性
                QXmlStreamAttributes attrs = xml.attributes();
                QString unicodeString = attrs.value(QLatin1String("UnicodeString")).toString();
                if (!unicodeString.isEmpty() && unicodeString.startsWith(QLatin1String("{}"))) {
                    unicodeString = unicodeString.mid(2);
                }

                // 检查属性形式的RenderTransform
                QString transformStr = attrs.value(QLatin1String("RenderTransform")).toString();
                QTransform glyphTransform = parentTransform;
                if (!transformStr.isEmpty()) {
                    QTransform attrTransform = parseTransformMatrix(transformStr);
                    glyphTransform = attrTransform * glyphTransform;
                }

                // 读取Glyphs的其他属性（用于创建GlyphInfo）
                bool okX = false, okY = false;
                double originX = attrs.value(QLatin1String("OriginX")).toDouble(&okX);
                double originY = attrs.value(QLatin1String("OriginY")).toDouble(&okY);
                bool okSize = false;
                double fontSize = attrs.value(QLatin1String("FontRenderingEmSize")).toDouble(&okSize);
                if (!okSize || fontSize <= 0) fontSize = 12.0;
                QString fontUri = attrs.value(QLatin1String("FontUri")).toString();
                QString indicesStr = attrs.value(QLatin1String("Indices")).toString();

                if (okX && okY && !unicodeString.isEmpty()) {
                    // 处理Glyphs的子元素，查找Glyphs.RenderTransform子元素
                    QTransform glyphLocalTransform; // 用于累积Glyphs.RenderTransform子元素的变换
                    bool inGlyphsRenderTransform = false;
                    int glyphDepth = 1; // Glyphs元素的嵌套深度

                    while (!xml.atEnd() && !xml.hasError()) {
                        QXmlStreamReader::TokenType nextToken = xml.readNext();
                        if (nextToken == QXmlStreamReader::StartElement) {
                            QString childName = xml.name().toString();
                            if (childName == QLatin1String("Glyphs")) {
                                glyphDepth++; // 嵌套的Glyphs
                            } else if (childName == QLatin1String("Glyphs.RenderTransform")) {
                                inGlyphsRenderTransform = true;
                                glyphLocalTransform = QTransform(); // 重置
                            } else if (childName == QLatin1String("MatrixTransform") && inGlyphsRenderTransform) {
                                // 在Glyphs.RenderTransform内部的MatrixTransform
                                // 注意：Qt的QTransform乘法：A * B 表示先应用B，再应用A
                                // 所以要累积多个MatrixTransform，应该使用：matrixTransform * glyphLocalTransform
                                QString matrixStr = xml.attributes().value(QLatin1String("Matrix")).toString();
                                if (!matrixStr.isEmpty()) {
                                    QTransform matrixTransform = parseTransformMatrix(matrixStr);
                                    glyphLocalTransform = matrixTransform * glyphLocalTransform;
                                }
                            }
                        } else if (nextToken == QXmlStreamReader::EndElement) {
                            QString childName = xml.name().toString();
                            if (childName == QLatin1String("Glyphs")) {
                                glyphDepth--;
                                if (glyphDepth == 0) {
                                    // 当前Glyphs元素结束
                                    break;
                                }
                            } else if (childName == QLatin1String("Glyphs.RenderTransform")) {
                                // Glyphs.RenderTransform子元素结束，应用累积的变换
                                glyphTransform = glyphLocalTransform * glyphTransform;
                                inGlyphsRenderTransform = false;
                            }
                        }
                    }

                    // 创建GlyphInfo
                    GlyphInfo glyph;
                    glyph.text = unicodeString;
                    glyph.position = QPointF(originX, originY);
                    glyph.fontSize = fontSize;
                    glyph.fontUri = fontUri;
                    glyph.transform = glyphTransform;
                    if (!indicesStr.isEmpty()) {
                        glyph.charWidths = parseIndices(indicesStr, unicodeString.length(), fontSize);
                    }
                    glyph.boundingBox = calculateGlyphBoundingBox(glyph);

                    if (!glyph.text.isEmpty()) {
                        TextRun run;
                        run.text = glyph.text;
                        run.boundingBox = glyph.boundingBox;
                        run.glyphs.append(glyph);
                        textRuns.append(run);
                    }
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
    // 根据libgxps (gxps-page.c:1012-1015)，UnicodeString可能以"{}"转义序列开头，需要跳过
    QString unicodeString = attrs.value(QLatin1String("UnicodeString")).toString();
    if (unicodeString.isEmpty()) {
        // 跳过空文本的Glyphs
        return glyph;
    }
    // 跳过开头的"{}"转义序列（根据libgxps的实现）
    if (unicodeString.startsWith(QLatin1String("{}"))) {
        unicodeString = unicodeString.mid(2);
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
            if (glyph.charWidths[i] >= 0) {
                // 有效的advanceWidth
                totalWidth += glyph.charWidths[i];
            } else {
                // -1.0标记表示glyphIndex（缺少advanceWidth），尝试从字体获取精确宽度
                if (i < glyph.text.length()) {
                    // calculateGlyphBoundingBox没有filePath参数，使用估算值作为fallback
                    totalWidth += estimateCharWidth(glyph.text.at(i), glyph.fontSize);
                }
            }
        }
    } else {
        // 如果没有Indices，使用估算
        for (int i = 0; i < glyph.text.length(); ++i) {
            totalWidth += estimateCharWidth(glyph.text.at(i), glyph.fontSize);
        }
    }

    double charAscent = glyph.fontSize * 0.75;
    double charDescent = glyph.fontSize * 0.25;
    double height = charAscent + charDescent;

    // 创建基础边界框（在文本位置，OriginY是文本基线位置）
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
    // 根据libgxps gxps-matrix.c:gxps_matrix_parse：
    // - 使用g_strsplit(data, ",", 6)分割成最多6个部分
    // - 检查g_strv_length(items) == 6确保正好6个值
    // - 使用g_ascii_strtod解析每个值（自动跳过前导空白）
    // - 如果解析失败或值无效，返回FALSE
    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QStringList parts = transformStr.split(QLatin1Char(','), QString::SkipEmptyParts);
    #else
    QStringList parts = transformStr.split(QLatin1Char(','), Qt::SkipEmptyParts);
    #endif
    if (parts.size() != 6) {
        return QTransform();
    }

    // 解析6个值
    bool ok = false;
    double m11 = parts[0].trimmed().toDouble(&ok);
    if (!ok) return QTransform();
    double m12 = parts[1].trimmed().toDouble(&ok);
    if (!ok) return QTransform();
    double m21 = parts[2].trimmed().toDouble(&ok);
    if (!ok) return QTransform();
    double m22 = parts[3].trimmed().toDouble(&ok);
    if (!ok) return QTransform();
    double dx = parts[4].trimmed().toDouble(&ok);
    if (!ok) return QTransform();
    double dy = parts[5].trimmed().toDouble(&ok);
    if (!ok) return QTransform();

    return QTransform(m11, m12, m21, m22, dx, dy);
}

double XpsTextExtractor::estimateCharWidth(QChar ch, double fontSize)
{
    if (fontSize <= 0) {
        return 12.0; // 默认宽度
    }

    // 字符宽度估算 (当无法从字体文件获取实际宽度时使用）
    // 中文字符、日文、韩文等全角字符
    if (ch.unicode() >= 0x4E00 && ch.unicode() <= 0x9FFF) { // CJK统一汉字
        return fontSize;
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

    #if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QStringList parts = indicesStr.split(QLatin1Char(';'), QString::SkipEmptyParts);
    #else
    QStringList parts = indicesStr.split(QLatin1Char(';'), Qt::SkipEmptyParts);
    #endif
    
    for (int i = 0; i < parts.size() && i < textLength; ++i) {
        QString part = parts[i].trimmed();
        if (part.isEmpty()) {
            // 空部分，使用估算值标记
            widths.append(-1.0);
            continue;
        }

        // 移除可能的逗号前缀（处理 ",100," 这种格式）
        while (part.startsWith(QLatin1Char(','))) {
            part = part.mid(1);
        }

        // 检查是否包含逗号（成对格式：glyphIndex,advanceWidth 或 glyphIndex,advanceWidth,h_offset,v_offset）
        QStringList commaParts = part.split(QLatin1Char(','));
        QString advanceWidthStr;

        if (commaParts.size() >= 2) {
            // 成对格式：第一个值是glyphIndex，第二个值（索引1）是advanceWidth
            advanceWidthStr = commaParts[1].trimmed();
        } else if (commaParts.size() == 1) {
            // 单独数字：根据libgxps的逻辑，这应该是glyphIndex，而不是advanceWidth
            // 当缺少advanceWidth时，使用-1.0标记，后续在extractWords中尝试从字体文件获取或使用估算值
            widths.append(-1.0);
            continue;
        } else {
            // 无法解析，使用估算值标记
            widths.append(-1.0);
            continue;
        }

        // 解析advanceWidth值
        advanceWidthStr = advanceWidthStr.trimmed();
        bool ok = false;
        double value = advanceWidthStr.toDouble(&ok);
        if (ok && value >= 0) {
            // advanceWidth的单位是1/100 em，实际宽度 = fontSize * value / 100.0
            double width = fontSize * value / 100.0;
            widths.append(width);
        } else {
            // 解析失败，使用估算值标记
            widths.append(-1.0);
        }
    }

    return widths;
}

QByteArray XpsTextExtractor::readFontFromZip(const QString &filePath, const QString &fontUri)
{
    if (filePath.isEmpty() || fontUri.isEmpty()) {
        return QByteArray();
    }

    QString cleanFontUri = fontUri;
    while (cleanFontUri.startsWith(QLatin1String("/")) || cleanFontUri.startsWith(QLatin1String("../"))) {
        if (cleanFontUri.startsWith(QLatin1String("/"))) {
            cleanFontUri = cleanFontUri.mid(1);
        } else if (cleanFontUri.startsWith(QLatin1String("../"))) {
            cleanFontUri = cleanFontUri.mid(3);
        }
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

        if (entryPath == fontUri || entryPath == cleanFontUri ||
            entryPath.endsWith(QLatin1String("/") + cleanFontUri) ||
            entryPath.contains(cleanFontUri.split(QLatin1String("/")).last())) {
            la_int64_t size = archive_entry_size(entry);
            if (size > 0 && size < 50 * 1024 * 1024) {
                result.resize(static_cast<int>(size));
                la_ssize_t readSize = archive_read_data(a, result.data(), size);
                if (readSize == size) {
                    found = true;
                } else {
                    result.clear();
                }
            }
            break;
        }
        archive_read_data_skip(a);
    }

    archive_read_free(a);

    if (!found) {
        return QByteArray();
    }

    return result;
}

} // namespace deepin_reader


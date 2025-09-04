// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "XpsDocument.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QBuffer>
#include <QDir>
#include <QFileInfo>
#include <QDateTime>
#include <QRegularExpression>
#include <QTextDocument>
#include <QFontMetrics>
#include <QProcess>
#include <QTemporaryDir>
#include <QDirIterator>
#include <QImageReader>
#include <QFontDatabase>
#include <cmath>
namespace deepin_reader {

XpsPage::XpsPage(const XpsPageData &pageData, QMutex *mutex)
    : m_pageData(pageData), m_docMutex(mutex)
{
}

XpsPage::~XpsPage()
{
}

QSizeF XpsPage::sizeF() const
{
    return m_pageData.size;
}

QImage XpsPage::render(int width, int height, const QRect &slice) const
{
    QMutexLocker lock(m_docMutex);
    
    QImage image(width, height, QImage::Format_RGB32);
    image.fill(Qt::white);
    
    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    
    const qreal margin = 10.0;
    const qreal pageW = m_pageData.size.width();
    const qreal pageH = m_pageData.size.height();

    const qreal contentW = qMax<qreal>(1.0, width  - 2.0 * margin);
    const qreal contentH = qMax<qreal>(1.0, height - 2.0 * margin);

    const qreal scaleX = contentW / pageW;
    const qreal scaleY = contentH / pageH;
    const qreal scale = qMin(scaleX, scaleY);

    const qreal drawW = pageW * scale;
    const qreal drawH = pageH * scale;
    const qreal offsetX = (width  - drawW) / 2.0;
    const qreal offsetY = (height - drawH) / 2.0;

    painter.translate(offsetX, offsetY);
    painter.scale(scale, scale);

    if (!slice.isNull()) {
        QRectF pageClip(slice);
        pageClip = pageClip.intersected(QRectF(0, 0, pageW, pageH));
        if (!pageClip.isEmpty()) {
            painter.setClipRect(pageClip);
            painter.translate(-pageClip.topLeft());
        }
    }
    
    // 保留最小化日志（如需更详细请在本处加回调试输出）
    
    for (int i = 0; i < m_pageData.paths.size(); ++i) {
        const QPainterPath &path = m_pageData.paths[i];
        if (i == 0) {
            painter.setPen(QPen(Qt::red, 2));
            painter.setBrush(QBrush(Qt::red));
        } else if (i == 1) {
            painter.setPen(QPen(Qt::green, 2));
            painter.setBrush(QBrush(Qt::green));
        } else {
            painter.setPen(QPen(Qt::black, 1));
            painter.setBrush(Qt::NoBrush);
        }
        
        painter.drawPath(path);
    }
    
    for (const auto &imgDraw : m_pageData.images) {
        if (!imgDraw.image.isNull()) {
            painter.drawImage(imgDraw.rect, imgDraw.image);
        }
    }

    for (int i = 0; i < m_pageData.textStrings.size(); ++i) {
        const QString &text = m_pageData.textStrings[i];
        if (text.trimmed().isEmpty()) continue;
        QPointF baseline = i < m_pageData.textOrigins.size() ? m_pageData.textOrigins[i] : QPointF();

        QColor textColor = (i < m_pageData.textColors.size()) ? m_pageData.textColors[i] : Qt::black;
        qreal fontSizeF = (i < m_pageData.textFontSizes.size()) ? m_pageData.textFontSizes[i] : 12.0;

        QFont font = painter.font();
        font.setPointSizeF(fontSizeF);
        font.setKerning(false);
        font.setLetterSpacing(QFont::PercentageSpacing, 100.0);
        if (i < m_pageData.textFontFamilies.size() && !m_pageData.textFontFamilies[i].isEmpty()) {
            font.setFamily(m_pageData.textFontFamilies[i]);
        }
        painter.setFont(font);
        painter.setPen(textColor);

        painter.setBrush(Qt::NoBrush);
        painter.setPen(textColor);

        qreal widthPx = painter.fontMetrics().horizontalAdvance(text);
        qreal heightPx = painter.fontMetrics().height();
        qreal widthPage = widthPx / scale;
        qreal heightPage = heightPx / scale;

        qreal indicesWidthPage = 0.0;
        if (i < m_pageData.textAdvances.size() && !m_pageData.textAdvances[i].isEmpty()) {
            const QVector<qreal> &adv = m_pageData.textAdvances[i];
            qreal x = baseline.x();
            const int n = text.size();
            const qreal desiredExtraTrackingEm = 0.10;

            QVector<qreal> baseCharWidths;
            baseCharWidths.reserve(n);
            qreal baseSum = 0.0;
            for (int k = 0; k < n; ++k) {
                const QString ch = text.mid(k, 1);
                qreal measured = painter.fontMetrics().horizontalAdvance(ch) / scale;
                qreal a = (k < adv.size()) ? adv[k] : 0.0;
                qreal fromIndices = (a / 100.0) * fontSizeF;
                qreal w = qMax(measured, fromIndices);
                baseCharWidths.append(w);
                baseSum += w;
            }

            qreal nextRunX = std::numeric_limits<qreal>::quiet_NaN();
            if (i + 1 < m_pageData.textOrigins.size()) {
                const qreal y = baseline.y();
                for (int j = i + 1; j < m_pageData.textOrigins.size(); ++j) {
                    if (m_pageData.textStrings[j].trimmed().isEmpty()) continue;
                    if (qAbs(m_pageData.textOrigins[j].y() - y) < 0.6) {
                        nextRunX = m_pageData.textOrigins[j].x();
                        break;
                    } else if (m_pageData.textOrigins[j].y() > y + 2.0) {
                        break;
                    }
                }
            }

            qreal extraPerChar = desiredExtraTrackingEm * fontSizeF;
            const qreal interRunGap = 0.10 * fontSizeF;
            if (!std::isnan(nextRunX)) {
                qreal allowed = nextRunX - interRunGap - baseline.x();
                if (allowed > 0) {
                    qreal room = allowed - baseSum;
                    if (room <= 0) {
                        extraPerChar = 0.0;
                    } else {
                        extraPerChar = qMin(extraPerChar, room / qMax(1, n));
                    }
                }
            }

            for (int k = 0; k < n; ++k) {
                const QString ch = text.mid(k, 1);
                painter.drawText(QPointF(x, baseline.y()), ch);
                
                qreal advancePage = baseCharWidths[k] + extraPerChar;
                
                QChar currentChar = ch.at(0);
                if (currentChar.unicode() >= 0x4E00 && currentChar.unicode() <= 0x9FFF) {
                    qreal chineseBonus = 0.02 * fontSizeF;
                    if (extraPerChar > 0) {
                        advancePage += chineseBonus;
                    }
                }
                
                x += advancePage;
                indicesWidthPage += advancePage;
            }
        } else {
            // 检查是否包含中文字符
            bool containsChinese = false;
            for (int k = 0; k < text.size(); ++k) {
                QChar ch = text.at(k);
                if (ch.unicode() >= 0x4E00 && ch.unicode() <= 0x9FFF) {
                    containsChinese = true;
                    break;
                }
            }
            
            QFont spaced = font;
            qreal letterSpacingPercent = containsChinese ? 112.0 : 108.0;
            spaced.setLetterSpacing(QFont::PercentageSpacing, letterSpacingPercent);
            painter.setFont(spaced);
            painter.drawText(baseline, text);
            indicesWidthPage = widthPage;
        }
    }
    return image;
}

Link XpsPage::getLinkAtPoint(const QPointF &)
{
    // XPS暂不支持链接
    return Link();
}

QString XpsPage::text(const QRectF &rect) const
{
    QString result;
    for (int i = 0; i < m_pageData.textRects.size() && i < m_pageData.textStrings.size(); ++i) {
        if (m_pageData.textRects[i].intersects(rect)) {
            result += m_pageData.textStrings[i] + " ";
        }
    }
    return result.trimmed();
}

QVector<PageSection> XpsPage::search(const QString &searchText, bool matchCase, bool wholeWords) const
{
    QVector<PageSection> results;
    
    QString searchPattern = searchText;
    if (!matchCase) {
        searchPattern = searchPattern.toLower();
    }
    
    for (int i = 0; i < m_pageData.textStrings.size(); ++i) {
        QString text = m_pageData.textStrings[i];
        if (!matchCase) {
            text = text.toLower();
        }
        
        if (wholeWords) {
            QRegularExpression regex(QString("\\b%1\\b").arg(QRegularExpression::escape(searchPattern)));
            if (regex.match(text).hasMatch()) {
                PageSection section;
                PageLine line;
                line.rect = m_pageData.textRects[i];
                line.text = m_pageData.textStrings[i];
                section.append(line);
                results.append(section);
            }
        } else {
            if (text.contains(searchPattern)) {
                PageSection section;
                PageLine line;
                line.rect = m_pageData.textRects[i];
                line.text = m_pageData.textStrings[i];
                section.append(line);
                results.append(section);
            }
        }
    }
    
    return results;
}

QList<Annotation *> XpsPage::annotations() const
{
    // XPS暂不支持注释
    return QList<Annotation *>();
}

QList<Word> XpsPage::words()
{
    if (m_wordLoaded) {
        return m_words;
    }
    
    m_words.clear();
    for (int i = 0; i < m_pageData.textStrings.size(); ++i) {
        QStringList wordList = m_pageData.textStrings[i].split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
        for (const QString &word : wordList) {
            if (!word.isEmpty()) {
                m_words.append(Word(word, m_pageData.textRects[i]));
            }
        }
    }
    
    m_wordLoaded = true;
    return m_words;
}

// XpsDocument实现
XpsDocument::XpsDocument(const QString &filePath)
    : m_filePath(filePath), m_loaded(false)
{
    qInfo() << "Creating XPS document for:" << filePath;
    loadXpsFile();
}

QByteArray XpsDocument::deobfuscateOdttf(const QByteArray &data, const QString &fontUri)
{
    QRegularExpression re("([0-9A-Fa-f]{8}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{4}-[0-9A-Fa-f]{12})");
    QRegularExpressionMatch m = re.match(fontUri);
    if (!m.hasMatch() || data.size() < 32) {
        return data;
    }
    QString guid = m.captured(1);
    guid.remove('-');
    if (guid.size() != 32) {
        return data;
    }
    QByteArray key;
    key.reserve(16);
    for (int i = 0; i < 16; ++i) {
        bool ok = false;
        char b = static_cast<char>(guid.mid(i * 2, 2).toUInt(&ok, 16));
        key.append(ok ? b : '\0');
    }
    QByteArray out = data;
    for (int i = 0; i < 32 && i < out.size(); ++i) {
        out[i] = out[i] ^ key[i % 16];
    }
    return out;
}

QString XpsDocument::resolveFontFamily(const QString &fontUri)
{
    if (fontUri.isEmpty()) return QString();
    if (m_fontFamilyCache.contains(fontUri)) {
        return m_fontFamilyCache.value(fontUri);
    }
    QString key = fontUri;
    if (key.startsWith('/')) key = key.mid(1);
    if (!m_archiveFiles.contains(key)) return QString();
    QByteArray raw = m_archiveFiles.value(key);
    QByteArray real = deobfuscateOdttf(raw, fontUri);
    int fontId = QFontDatabase::addApplicationFontFromData(real);
    if (fontId == -1) {
        qWarning() << "Failed to add embedded font from" << key;
        return QString();
    }
    m_loadedFontIds.append(fontId);
    const QStringList fams = QFontDatabase::applicationFontFamilies(fontId);
    if (fams.isEmpty()) return QString();
    const QString fam = fams.first();
    m_fontFamilyCache.insert(fontUri, fam);
    qInfo() << "Loaded embedded font" << fontUri << "-> family" << fam;
    return fam;
}

XpsDocument::~XpsDocument()
{
}

int XpsDocument::pageCount() const
{
    return m_pages.size();
}

Page *XpsDocument::page(int index) const
{
    if (index >= 0 && index < m_pages.size()) {
        return new XpsPage(m_pages[index], const_cast<QMutex *>(&m_mutex));
    }
    return nullptr;
}

QStringList XpsDocument::saveFilter() const
{
    return QStringList() << "XPS files (*.xps)";
}

bool XpsDocument::save() const
{
    // XPS暂不支持保存
    return false;
}

bool XpsDocument::saveAs(const QString &filePath) const
{
    // XPS暂不支持另存为
    Q_UNUSED(filePath)
    return false;
}

Outline XpsDocument::outline() const
{
    // XPS暂不支持大纲
    return Outline();
}

Properties XpsDocument::properties() const
{
    Properties props;
    props["Title"] = m_title;
    props["Author"] = m_author;
    props["Subject"] = m_subject;
    props["Creator"] = m_creator;
    props["CreationDate"] = m_creationDate;
    props["ModificationDate"] = m_modificationDate;
    props["PageCount"] = m_pages.size();
    props["Width"] = m_documentSize.width();
    props["Height"] = m_documentSize.height();
    return props;
}

XpsDocument *XpsDocument::loadDocument(const QString &filePath, Document::Error &error)
{
    qInfo() << "Loading XPS document:" << filePath;
    
    XpsDocument *doc = new XpsDocument(filePath);
    if (doc->m_loaded) {
        error = Document::NoError;
        return doc;
    } else {
        error = Document::FileError;
        delete doc;
        return nullptr;
    }
}

bool XpsDocument::loadXpsFile()
{
    qInfo() << "Loading XPS file:" << m_filePath;
    
    // 创建临时目录来解压XPS文件
    QTemporaryDir tempDir;
    if (!tempDir.isValid()) {
        qCritical() << "Failed to create temporary directory for XPS extraction";
        return false;
    }
    
    // 使用系统unzip命令解压XPS文件
    QProcess unzipProcess;
    unzipProcess.setWorkingDirectory(tempDir.path());
    
    QStringList arguments;
    arguments << "-q" << "-o" << QFileInfo(m_filePath).absoluteFilePath(); // 使用绝对路径
    
    unzipProcess.start("unzip", arguments);
    
    if (!unzipProcess.waitForStarted()) {
        qCritical() << "Failed to start unzip process";
        return false;
    }
    
    if (!unzipProcess.waitForFinished()) {
        qCritical() << "Unzip process failed";
        return false;
    }
    
    if (unzipProcess.exitCode() != 0) {
        qCritical() << "Unzip process failed with exit code:" << unzipProcess.exitCode();
        qCritical() << "Unzip error output:" << unzipProcess.readAllStandardError();
        return false;
    }
    
    // 读取解压后的文件，包括子目录
    QDir extractedDir(tempDir.path());
    QFileInfoList allFiles;
    
    // 递归获取所有文件
    // 仅在需要时查看临时目录
    
    
    // 先检查临时目录中的内容
    QDir tempDirObj(tempDir.path());
    QFileInfoList tempContents = tempDirObj.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    for (const QFileInfo &item : tempContents) {
        
    }
    
    // 使用递归函数来获取所有文件
    QList<QPair<QString, QString>> fileList; // <relativePath, absolutePath>
    collectFilesRecursively(tempDir.path(), "", fileList);
    
    
    for (const auto &filePair : fileList) {
        
    }
    
    for (const auto &filePair : fileList) {
        QString relativePath = filePair.first;
        QString absolutePath = filePair.second;
        
        QFile file(absolutePath);
        if (file.open(QIODevice::ReadOnly)) {
            
            
            QByteArray fileData = file.readAll();
            m_archiveFiles[relativePath] = fileData;
            
            file.close();
        }
    }
    
    
    
    parseXpsContent();
    m_loaded = true;
    return true;
}

void XpsDocument::collectFilesRecursively(const QString &dirPath, const QString &relativePath, QList<QPair<QString, QString>> &fileList)
{
    QDir dir(dirPath);
    QFileInfoList entries = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
    
    for (const QFileInfo &entry : entries) {
        QString newRelativePath = relativePath.isEmpty() ? entry.fileName() : relativePath + "/" + entry.fileName();
        
        if (entry.isFile()) {
            fileList.append(qMakePair(newRelativePath, entry.absoluteFilePath()));
        } else if (entry.isDir()) {
            collectFilesRecursively(entry.absoluteFilePath(), newRelativePath, fileList);
        }
    }
}

void XpsDocument::parseXpsContent()
{
    qInfo() << "Parsing XPS content";
    
    // 查找 fdseq：兼容 FixedDocumentSequence.fdseq 与 FixedDocSeq.fdseq，且不限定具体目录
    QString fdseqKey;
    const QStringList keys = m_archiveFiles.keys();
    for (const QString &key : keys) {
        const QString base = QFileInfo(key).fileName();
        if (base.compare("FixedDocumentSequence.fdseq", Qt::CaseInsensitive) == 0 ||
            base.compare("FixedDocSeq.fdseq", Qt::CaseInsensitive) == 0) {
            fdseqKey = key;
            break;
        }
    }
    if (fdseqKey.isEmpty()) {
        qWarning() << "fdseq not found. Available keys:" << keys;
        return;
    }
    
    
    QByteArray fdseqData = m_archiveFiles[fdseqKey];
    QXmlStreamReader reader(fdseqData);
    
    QString documentRef;
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "DocumentReference") {
                QXmlStreamAttributes attrs = reader.attributes();
                documentRef = attrs.value("Source").toString();
                break;
            }
        }
    }
    
    if (documentRef.isEmpty()) {
        qWarning() << "Document reference not found";
        return;
    }
    
    // 解析主文档
    // 移除开头的斜杠，因为我们的文件键不包含开头的斜杠
    QString docKey = documentRef;
    if (docKey.startsWith("/")) {
        docKey = docKey.mid(1);
    }
    
    if (!m_archiveFiles.contains(docKey)) {
        qWarning() << "Main document not found:" << docKey << "(original:" << documentRef << ")";
        
        return;
    }
    
    QByteArray docData = m_archiveFiles[docKey];
    QXmlStreamReader docReader(docData);
    
    while (!docReader.atEnd()) {
        QXmlStreamReader::TokenType token = docReader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (docReader.name() == "PageContent") {
                QXmlStreamAttributes attrs = docReader.attributes();
                QString pageSource = attrs.value("Source").toString();

                

                // 解析页面路径：
                // - 以 '/' 开头：包内绝对路径，直接去掉开头的 '/'
                // - 否则：相对于主文档 FixedDoc.fdoc 所在目录
                QString pageKey;
                if (pageSource.startsWith('/')) {
                    pageKey = pageSource.mid(1);
                } else {
                    const QString baseDir = QFileInfo(docKey).path();
                    pageKey = QDir(baseDir).filePath(pageSource);
                    // 统一为正斜杠
                    pageKey.replace("\\", "/");
                }
                
                // 解析页面
                if (m_archiveFiles.contains(pageKey)) {
                    qInfo() << "Parsing page:" << pageKey;
                    XpsPageData pageInfo;
                    parsePageContent(m_archiveFiles[pageKey], pageInfo);
                    m_pages.append(pageInfo);
                } else {
                    qWarning() << "Page source not found:" << pageKey;
                    
                }
            }
        }
    }
    
    // 设置文档属性
    if (!m_pages.isEmpty()) {
        m_documentSize = m_pages.first().size;
        qInfo() << "XPS document loaded with" << m_pages.size() << "pages";
    }
}

void XpsDocument::parsePageContent(const QByteArray &pageData, XpsPageData &pageInfo)
{
    QXmlStreamReader reader(pageData);
    
    // 获取页面尺寸
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "FixedPage") {
                QXmlStreamAttributes attrs = reader.attributes();
                QString widthStr = attrs.value("Width").toString();
                QString heightStr = attrs.value("Height").toString();
                
                bool ok1, ok2;
                qreal width = widthStr.toDouble(&ok1);
                qreal height = heightStr.toDouble(&ok2);
                
                if (ok1 && ok2) {
                    pageInfo.size = QSizeF(width, height);
                    
                } else {
                    pageInfo.size = QSizeF(816, 1056); // 默认A4尺寸
                    qWarning() << "Using default page size:" << pageInfo.size;
                }
                break;
            }
        }
    }
    
    // 重置读取器
    reader.clear();
    reader.addData(pageData);
    
    // 解析页面内容
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "Path") {
                QPainterPath path;
                parsePathData(reader, path, pageInfo);
                if (!path.isEmpty()) {
                    pageInfo.paths.append(path);
                    
                }
            } else if (reader.name() == "Glyphs") {
                parseTextData(reader, pageInfo);
            }
        }
    }
    
    
}

void XpsDocument::parsePathData(QXmlStreamReader &reader, QPainterPath &path, XpsPageData &pageInfo)
{
    QXmlStreamAttributes attrs = reader.attributes();
    QString fillColor = attrs.value("Fill").toString();
    
    
    
    // 解析路径数据
    while (!reader.atEnd()) {
        QXmlStreamReader::TokenType token = reader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (reader.name() == "Path.Data") {
                // 解析路径数据子元素
                while (!reader.atEnd()) {
                    token = reader.readNext();
                    if (token == QXmlStreamReader::StartElement) {
                        if (reader.name() == "RectangleGeometry") {
                            QXmlStreamAttributes rectAttrs = reader.attributes();
                            QString rectStr = rectAttrs.value("Rect").toString();
                            QStringList coords = rectStr.split(",");
                            if (coords.size() == 4) {
                                qreal x = coords[0].toDouble();
                                qreal y = coords[1].toDouble();
                                qreal w = coords[2].toDouble();
                                qreal h = coords[3].toDouble();
                                path.addRect(x, y, w, h);
                                    
                            }
                        } else if (reader.name() == "EllipseGeometry") {
                            QXmlStreamAttributes ellipseAttrs = reader.attributes();
                            QString centerStr = ellipseAttrs.value("Center").toString();
                            QString radiusXStr = ellipseAttrs.value("RadiusX").toString();
                            QString radiusYStr = ellipseAttrs.value("RadiusY").toString();
                            
                            QStringList centerCoords = centerStr.split(",");
                            if (centerCoords.size() == 2) {
                                qreal centerX = centerCoords[0].toDouble();
                                qreal centerY = centerCoords[1].toDouble();
                                qreal radiusX = radiusXStr.toDouble();
                                qreal radiusY = radiusYStr.toDouble();
                                
                                path.addEllipse(centerX - radiusX, centerY - radiusY, 
                                              radiusX * 2, radiusY * 2);
                                
                            }
                        }
                    }
                    if (token == QXmlStreamReader::EndElement && reader.name() == "Path.Data") {
                        break;
                    }
                }
                break;
            } else if (reader.name() == "Path.Fill") {
                // 解析 Path.Fill -> ImageBrush -> ImageBrush.Transform -> MatrixTransform
                while (!reader.atEnd()) {
                    token = reader.readNext();
                    if (token == QXmlStreamReader::StartElement && reader.name() == "ImageBrush") {
                        QXmlStreamAttributes battrs = reader.attributes();
                        QString imageSource = battrs.value("ImageSource").toString();

                        // 默认矩阵为单位矩阵
                        qreal m11 = 1.0, m12 = 0.0, m21 = 0.0, m22 = 1.0, dx = 0.0, dy = 0.0;

                        int depth = 1;
                        while (!reader.atEnd() && depth > 0) {
                            token = reader.readNext();
                            if (token == QXmlStreamReader::StartElement) {
                                if (reader.name() == "MatrixTransform") {
                                    QString matrix = reader.attributes().value("Matrix").toString();
                                    QStringList ps = matrix.split(',');
                                    if (ps.size() == 6) {
                                        m11 = ps[0].toDouble();
                                        m12 = ps[1].toDouble();
                                        m21 = ps[2].toDouble();
                                        m22 = ps[3].toDouble();
                                        dx = ps[4].toDouble();
                                        dy = ps[5].toDouble();
                                    }
                                }
                                depth++;
                            } else if (token == QXmlStreamReader::EndElement) {
                                depth--;
                            }
                            if (token == QXmlStreamReader::EndElement && reader.name() == "ImageBrush") {
                                break;
                            }
                        }

                        if (!imageSource.isEmpty()) {
                            QString key = imageSource;
                            if (key.startsWith('/')) key = key.mid(1);
                            if (m_archiveFiles.contains(key)) {
                                QImage img;
                                img.loadFromData(m_archiveFiles[key]);
                                if (!img.isNull()) {
                                    const qreal scaleBoost = 1.15;
                                    QRectF rect(dx, dy, m11 * scaleBoost, m22 * scaleBoost);
                                    XpsPageData::XpsImageDraw draw{img, rect};
                                    pageInfo.images.append(draw);
                                    
                                } else {
                                    qWarning() << "Failed to decode image from" << key;
                                }
                            } else {
                                qWarning() << "Image source not found in archive:" << key;
                            }
                        }
                    }
                    if (token == QXmlStreamReader::EndElement && reader.name() == "Path.Fill") {
                        break;
                    }
                }
            }
        }
        if (token == QXmlStreamReader::EndElement && reader.name() == "Path") {
            break;
        }
    }
}

void XpsDocument::parseTextData(QXmlStreamReader &reader, XpsPageData &pageInfo)
{
    // 解析文本属性
    QXmlStreamAttributes attrs = reader.attributes();
    QString originX = attrs.value("OriginX").toString();
    QString originY = attrs.value("OriginY").toString();
    QString fontSize = attrs.value("FontRenderingEmSize").toString();
    QString fontUri = attrs.value("FontUri").toString();
    QString fill = attrs.value("Fill").toString(); // 可能是 #AARRGGBB 或 rgb() 等
    
    
    
    // 读取文本内容：优先属性 UnicodeString，其次子元素 UnicodeString
    QString text = attrs.value("UnicodeString").toString();
    if (text.isEmpty()) {
        while (!reader.atEnd()) {
            QXmlStreamReader::TokenType token = reader.readNext();
            if (token == QXmlStreamReader::StartElement && reader.name() == "UnicodeString") {
                text = reader.readElementText();
                break;
            }
            if (token == QXmlStreamReader::EndElement && reader.name() == "Glyphs") {
                break;
            }
        }
    }
    
    if (!text.isEmpty()) {
        
        
        bool ok1, ok2, ok3;
        qreal x = originX.toDouble(&ok1);
        qreal y = originY.toDouble(&ok2);
        qreal size = fontSize.toDouble(&ok3);
        
        if (ok1 && ok2 && ok3) {
            // 记录基线点和字号；矩形顶部按近似 ascent（0.88em），高度按约 1.15em，
            // 这样与实际文本基线位置更贴合，避免 y 坐标偏移
            QRectF textRect(x, y - size * 0.88, text.length() * size * 0.6, size * 1.15);
            pageInfo.textRects.append(textRect);
            pageInfo.textStrings.append(text);
            pageInfo.textOrigins.append(QPointF(x, y));
            pageInfo.textFontSizes.append(size);
            pageInfo.textFontUris.append(fontUri);
            // 尝试加载嵌入字体，记录 family
            QString fam = resolveFontFamily(fontUri);
            pageInfo.textFontFamilies.append(fam);

            // 解析颜色
            QColor color = Qt::black;
            if (!fill.isEmpty()) {
                if (fill.startsWith('#')) {
                    color = QColor(fill);
                } else if (fill.startsWith("rgb")) {
                    // 简单处理 rgb(r,g,b) / rgba(r,g,b,a)
                    QString inside = fill.mid(fill.indexOf('(') + 1);
                    inside.chop(1);
                    QStringList comps = inside.split(',');
                    if (comps.size() >= 3) {
                        int r = comps[0].trimmed().toInt();
                        int g = comps[1].trimmed().toInt();
                        int b = comps[2].trimmed().toInt();
                        int a = 255;
                        if (comps.size() == 4) {
                            a = static_cast<int>(comps[3].trimmed().toDouble() * 255);
                        }
                        color = QColor(r, g, b, a);
                    }
                }
            }
            pageInfo.textColors.append(color);

            // 解析 Glyphs.Indices（示例：",54.545;,30.682;,..."），这里仅提取数值作为 advance 百分比（1/100 em）
            QVector<qreal> advances;
            QString idx = attrs.value("Indices").toString();
            if (!idx.isEmpty()) {
                // 去掉分号，将逗号分隔的数值提取
                QString cleaned = idx;
                cleaned.replace(';', ' ');
                const QStringList parts = cleaned.split(',', Qt::SkipEmptyParts);
                for (const QString &part : parts) {
                    bool okv = false;
                    qreal v = part.trimmed().toDouble(&okv);
                    if (okv) advances.append(v);
                }
            }
            pageInfo.textAdvances.append(advances);

            
        }
    }
}

} // namespace deepin_reader

// Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef XPSDOCUMENT_H
#define XPSDOCUMENT_H

#include "Model.h"

#include <QCoreApplication>
#include <QMutex>
#include <QScopedPointer>
#include <QPointF>
#include <QRectF>
#include <QImage>
#include <QSizeF>
#include <QList>
#include <QString>
#include <QXmlStreamReader>
#include <QBuffer>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QVector>

class QPainter;
class QPainterPath;

namespace deepin_reader {

// XPS页面数据结构
struct XpsPageData {
    QSizeF size;
    QString content;
    QList<QPainterPath> paths;
    QList<QRectF> textRects;
    QList<QString> textStrings;
    QList<QPointF> textOrigins; // Glyphs 原始基线坐标 (OriginX, OriginY)
    QList<qreal> textFontSizes; // 字号，用于按基线渲染
    QList<QColor> textColors;   // 文本颜色
    QList<QString> textFontUris; // 字体URI（用于后续映射字体族）
    QList<QString> textFontFamilies; // 解析并加载后的字体家族名称
    QList<QVector<qreal>> textAdvances; // 来自 Glyphs.Indices 的 advance（单位：1/100 em）
    QImage thumbnail;
    struct XpsImageDraw {
        QImage image;
        QRectF rect;
    };
    QList<XpsImageDraw> images;
};

// XPS页面类，继承自Page接口
class XpsPage : public Page
{
    Q_DISABLE_COPY(XpsPage)

    friend class XpsDocument;

public:
    ~XpsPage() override;

    QSizeF sizeF() const override;
    QImage render(int width, int height, const QRect &slice = QRect()) const override;
    Link getLinkAtPoint(const QPointF &point) override;
    QString text(const QRectF &rect) const override;
    QVector<PageSection> search(const QString &text, bool matchCase, bool wholeWords) const override;
    QList<Annotation *> annotations() const override;
    QList<Word> words() override;

private:
    explicit XpsPage(const XpsPageData &pageData, QMutex *mutex);

    XpsPageData m_pageData;
    QMutex *m_docMutex = nullptr;
    bool m_wordLoaded = false;
    QList<Word> m_words;
};

// XPS文档类，继承自Document接口
class XpsDocument : public Document
{
    Q_DISABLE_COPY(XpsDocument)

public:
    explicit XpsDocument(const QString &filePath);
    ~XpsDocument() override;

    // Document接口实现
    int pageCount() const override;
    Page *page(int index) const override;
    QStringList saveFilter() const override;
    bool save() const override;
    bool saveAs(const QString &filePath) const override;
    Outline outline() const override;
    Properties properties() const override;

    // 静态加载函数
    static XpsDocument *loadDocument(const QString &filePath, Document::Error &error);

private:
    // XPS解析和渲染相关成员函数
    bool loadXpsFile();
    void parseXpsContent();
    QImage renderPage(int pageIndex, int width, int height) const;
    void parsePageContent(const QByteArray &pageData, XpsPageData &pageInfo);
                    void parsePathData(QXmlStreamReader &reader, QPainterPath &path, XpsPageData &pageInfo);
                void parseTextData(QXmlStreamReader &reader, XpsPageData &pageInfo);
                void collectFilesRecursively(const QString &dirPath, const QString &relativePath, QList<QPair<QString, QString>> &fileList);
                QString resolveFontFamily(const QString &fontUri);
                static QByteArray deobfuscateOdttf(const QByteArray &data, const QString &fontUri);

    QString m_filePath;
    QList<XpsPageData> m_pages;
    QMutex m_mutex;
    bool m_loaded;
    QMap<QString, QByteArray> m_archiveFiles;
    QSizeF m_documentSize;
    QMap<QString, QString> m_fontFamilyCache; // FontUri -> family
    QList<int> m_loadedFontIds; // 保持字体生命周期
    QString m_title;
    QString m_author;
    QString m_subject;
    QString m_creator;
    QDateTime m_creationDate;
    QDateTime m_modificationDate;
};

} // namespace deepin_reader

#endif // XPSDOCUMENT_H

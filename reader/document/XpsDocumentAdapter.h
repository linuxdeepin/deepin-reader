// Copyright (C) 2019 ~ 2025 Uniontech Software Technology Co.,Ltd.
// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef XPSDOCUMENTADAPTER_H
#define XPSDOCUMENTADAPTER_H

#include "Model.h"

#include <QImage>
#include <QMutex>
#include <QSizeF>
#include <QVector>
#include <memory>

struct _GXPSFile;
struct _GXPSDocument;
struct _GXPSDocumentStructure;
struct _GXPSOutlineIter;

namespace deepin_reader {

class XpsDocumentAdapter : public Document
{
    Q_OBJECT
public:
    static XpsDocumentAdapter *loadDocument(const QString &filePath, Document::Error &error);

    ~XpsDocumentAdapter() override;

    int pageCount() const override;
    Page *page(int index) const override;
    QStringList saveFilter() const override;
    bool save() const override;
    bool saveAs(const QString &filePath) const override;
    Outline outline() const override;
    Properties properties() const override;

    QImage renderPage(int pageIndex, int width, int height, const QRect &slice) const;
    QSizeF pageSize(int pageIndex) const;

    QString filePath() const { return m_filePath; }

private:
    class Handle;

    XpsDocumentAdapter(const QString &filePath,
                       _GXPSFile *fileHandle,
                       _GXPSDocument *documentHandle);

    void ensurePageCache() const;
    void ensureProperties() const;
    void ensureOutline() const;

    QString m_filePath;
    mutable QMutex m_mutex;
    mutable QVector<QSizeF> m_pageSizes;
    mutable Properties m_properties;
    mutable Outline m_outline;
    std::unique_ptr<Handle> m_handle;
};

class XpsPageAdapter : public Page
{
    Q_OBJECT
public:
    XpsPageAdapter(const XpsDocumentAdapter *document, int pageIndex);
    ~XpsPageAdapter() override;

    QSizeF sizeF() const override;
    QImage render(int width, int height, const QRect &slice = QRect()) const override;
    Link getLinkAtPoint(const QPointF &point) override;
    QString text(const QRectF &rect) const override;
    QVector<PageSection> search(const QString &text, bool matchCase, bool wholeWords) const override;
    QList<Annotation *> annotations() const override;
    QList<Word> words() override;

private:
    const XpsDocumentAdapter *m_document;
    int m_pageIndex = -1;
};

} // namespace deepin_reader

#endif // XPSDOCUMENTADAPTER_H


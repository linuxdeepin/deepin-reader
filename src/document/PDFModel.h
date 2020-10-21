/*
* Copyright (C) 2019 ~ 2020 Uniontech Software Technology Co.,Ltd.
*
* Author:     qpdfview
*
* Maintainer: zhangsong<zhangsong@uniontech.com>
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
#ifndef PDFMODEL_H
#define PDFMODEL_H

#include "Model.h"

#include <QCoreApplication>
#include <QMutex>
#include <QScopedPointer>
#include <QPointF>
#include <QRectF>

class DPdfDoc;
class DPdfPage;
class DPdfAnnot;

namespace deepin_reader {
class PDFAnnotation : public Annotation
{
    Q_OBJECT

    friend class PDFPage;

public:
    ~PDFAnnotation() override;

    QList<QRectF> boundary() const override;

    QString contents() const override;

    int type() override;

    DPdfAnnot *ownAnnotation() override;

private:
    Q_DISABLE_COPY(PDFAnnotation)

    PDFAnnotation(DPdfAnnot *annotation);

    DPdfAnnot *m_annotation;

};

class PDFPage : public Page
{
    Q_DECLARE_TR_FUNCTIONS(PDFPage)

    friend class PDFDocument;

public:
    ~PDFPage() override;

    QSizeF sizeF() const override;

    QImage render(const qreal &scaleFactor) const override;

    QImage render(Dr::Rotation rotation, const double scaleFactor, const QRectF &boundingRect = QRectF()) const override;

    QImage render(int width, int height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio) const override;

    QImage render(qreal horizontalResolution, qreal verticalResolution, Dr::Rotation rotation, QRectF boundingRect) const;

    Link getLinkAtPoint(const QPointF &point) const override;

    QString text(const QRectF &rect) const override;

    QList<Word> words() override;

    QVector<QRectF> search(const QString &text, bool matchCase, bool wholeWords) const override;

    QList< Annotation * > annotations() const override;

    Annotation *addHighlightAnnotation(const QList<QRectF> &boundarys, const QString &text, const QColor &color) override;

    bool removeAnnotation(deepin_reader::Annotation *annotation) override;

    bool updateAnnotation(Annotation *annotation, const QString &, const QColor &) override;

    bool mouseClickIconAnnot(QPointF &) override;

    Annotation *addIconAnnotation(const QRectF &ponit, const QString &text) override;

    Annotation *moveIconAnnotation(Annotation *annot, const QRectF &rect) override;

private:
    Q_DISABLE_COPY(PDFPage)

    PDFPage(QMutex *mutex, DPdfPage *page);

    QMutex *m_docMutex = nullptr;

    DPdfPage *m_page = nullptr;

    QList<Word> m_words;

    QSizeF m_pageSizef;
};

class PDFDocument : public Document
{
    Q_DECLARE_TR_FUNCTIONS(Model::PDFDocument)

public:
    virtual ~PDFDocument() override;

    int numberOfPages() const override;

    Page *page(int index) const override;

    QStringList saveFilter() const override;

    QString label(int index) const override;

    QSizeF pageSizeF(int index) const override;

    bool save(const QString &filePath) const override;

    bool saveAs(const QString &filePath) const override;

    Outline outline() const override;

    Properties properties() const override;

    static PDFDocument *loadDocument(const QString &filePath, const QString &password);

    static int tryLoadDocument(const QString &filePath, const QString &password);

private:
    Q_DISABLE_COPY(PDFDocument)

    explicit PDFDocument(DPdfDoc *document);

    DPdfDoc *m_document = nullptr;

    QMutex *m_docMutex;

    mutable Properties m_fileProperties;

    mutable Outline m_outline;
};
}


#endif // PDFMODEL_H

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
    Q_DISABLE_COPY(PDFAnnotation)

    friend class PDFPage;

public:
    ~PDFAnnotation() override;

    QList<QRectF> boundary() const override;

    QString contents() const override;

    int type() override;

    DPdfAnnot *ownAnnotation() override;

private:
    explicit PDFAnnotation(DPdfAnnot *dannotation);

    DPdfAnnot *m_dannotation;

};

class PDFPage : public Page
{
    Q_DISABLE_COPY(PDFPage)

    friend class PDFDocument;
public:
    ~PDFPage() override;

    /**
     * @brief 获取大小 会根据当前设备的像素密度保持每一页在不同设备上大小相同.
     * @return
     */
    QSizeF sizeF() const override;

    QImage render(int width, int height, const QRect &slice = QRect()) const override;

    Link getLinkAtPoint(const QPointF &point) override;

    QString text(const QRectF &rect) const override;

    QList<Word> words() override;

    QVector<QRectF> search(const QString &text, bool matchCase, bool wholeWords) const override;

    QList< Annotation * > annotations() const override;

    Annotation *addHighlightAnnotation(const QList<QRectF> &boundaries, const QString &text, const QColor &color) override;

    bool removeAnnotation(Annotation *annotation) override;

    bool updateAnnotation(Annotation *annotation, const QString &, const QColor &) override;

    Annotation *addIconAnnotation(const QRectF &ponit, const QString &text) override;

    Annotation *moveIconAnnotation(Annotation *annot, const QRectF &rect) override;

private:
    explicit PDFPage(QMutex *mutex, DPdfPage *page);

    QMutex *m_docMutex = nullptr;

    DPdfPage *m_page = nullptr;

    bool m_wordLoaded = false;

    QList<Word> m_words;
};

class PDFDocument : public Document
{
    Q_DISABLE_COPY(PDFDocument)
public:
    explicit PDFDocument(DPdfDoc *document);

    virtual ~PDFDocument() override;

    int pageCount() const override;

    Page *page(int index) const override;

    QStringList saveFilter() const override;

    QString label(int index) const override;

    bool save() const override;

    bool saveAs(const QString &filePath) const override;

    Outline outline() const override;

    Properties properties() const override;

    static PDFDocument *loadDocument(const QString &filePath, const QString &password, deepin_reader::Document::Error &error);

private:
    DPdfDoc *m_document = nullptr;

    QMutex *m_docMutex = nullptr;

    mutable Properties m_fileProperties;

    mutable Outline m_outline;

    qreal m_xRes = 72;

    qreal m_yRes = 72;
};
}


#endif // PDFMODEL_H

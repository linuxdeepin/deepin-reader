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

namespace Poppler {
class Annotation;
class Document;
class FormField;
class Page;
}

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

    Poppler::Annotation *ownAnnotation() override;

private:
    Q_DISABLE_COPY(PDFAnnotation)

    PDFAnnotation(QMutex *mutex, Poppler::Annotation *annotation);

    mutable QMutex *m_mutex;

    Poppler::Annotation *m_annotation;

};

class PDFPage : public Page
{
    Q_DECLARE_TR_FUNCTIONS(PDFPage)

    friend class PDFDocument;

public:
    ~PDFPage();

    QSize size() const;

    QSizeF sizeF() const;

    QImage render(Dr::Rotation rotation = Dr::RotateBy0, const double scaleFactor = 1.00, const QRect &boundingRect = QRect()) const;

    QImage render(int width, int height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio) const;

    QImage render(qreal horizontalResolution, qreal verticalResolution, Dr::Rotation rotation, QRect boundingRect) const;

    QString label() const;

    QList< Link * > links() const;

    QString text(const QRectF &rect) const;

    QString cachedText(const QRectF &rect) const;

    QList<Word> words(Dr::Rotation rotation)const;

    QList< QRectF > search(const QString &text, bool matchCase, bool wholeWords) const;

    QList< Annotation * > annotations() const;

    bool canAddAndRemoveAnnotations() const;

    Annotation *addTextAnnotation(const QRectF &boundary, const QColor &color);                 //跳转注释

    Annotation *addHighlightAnnotation(const QRectF &boundary, const QColor &color);            //高亮

    Annotation *addHighlightAnnotation(const QList<QRectF> &boundarys, const QString &text, const QColor &color);

    bool removeAnnotation(Annotation *annotation);

    QList< FormField * > formFields() const;

    bool updateAnnotation(Annotation *annotation, const QString &, const QColor &);

    bool mouseClickIconAnnot(QPointF &) override;

    Annotation *addIconAnnotation(const QRectF ponit, const QString text) override;
private:
    Q_DISABLE_COPY(PDFPage)

    PDFPage(QMutex *mutex, Poppler::Page *page);

    mutable QMutex *m_mutex;

    Poppler::Page *m_page;

};

class PDFDocument : public Document
{
    Q_DECLARE_TR_FUNCTIONS(Model::PDFDocument)

public:
    ~PDFDocument();

    int numberOfPages() const;

    Page *page(int index) const;

    bool isLocked() const;

    bool unlock(const QString &password);

    QStringList saveFilter() const;

    bool canSave() const;

    bool save(const QString &filePath, bool withChanges) const;

    bool canBePrintedUsingCUPS() const;

    void setPaperColor(const QColor &paperColor);

    Outline outline() const;

    Properties properties() const;

    QAbstractItemModel *fonts() const;

    bool wantsContinuousMode() const;

    bool wantsSinglePageMode() const;

    bool wantsTwoPagesMode() const;

    bool wantsTwoPagesWithCoverPageMode() const;

    bool wantsRightToLeftMode() const;

    static deepin_reader::PDFDocument *loadDocument(const QString &filePath);

private:
    Q_DISABLE_COPY(PDFDocument)

    PDFDocument(Poppler::Document *document);

    mutable QMutex m_mutex;

    Poppler::Document *m_document;
};
}


#endif // PDFMODEL_H

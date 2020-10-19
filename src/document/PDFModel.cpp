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
#include "PDFModel.h"
#include "deepin-pdfium"

#include <QDebug>

#define LOCK_DOCUMENT QMutexLocker mutexLocker(&m_mutex);
#define LOCK_PAGE QMutexLocker mutexLocker(m_mutex);

namespace deepin_reader {

PDFAnnotation::PDFAnnotation(QMutex *mutex, DPdfAnnot *annotation) : Annotation(),
    m_mutex(mutex),
    m_annotation(annotation)
{
}

PDFAnnotation::~PDFAnnotation()
{
    if (m_annotation) {
        delete m_annotation;
        m_annotation = nullptr;
    }
}

QList<QRectF> PDFAnnotation::boundary() const
{
    QList<QRectF> rectFList;

    if (m_annotation->type() == DPdfAnnot::AText || m_annotation->type() == DPdfAnnot::AHighlight) {
        rectFList.append(m_annotation->boundaries());
    }
    return rectFList;
}

QString PDFAnnotation::contents() const
{
    if (nullptr == m_annotation)
        return QString();

    return m_annotation->text();
}

int PDFAnnotation::type()
{
    if (nullptr == m_annotation)
        return -1;

    return m_annotation->type();
}

DPdfAnnot *PDFAnnotation::ownAnnotation()
{
    return m_annotation;
}

PDFPage::PDFPage(QMutex *mutex, DPdfPage *page) :
    m_mutex(mutex),
    m_page(page)
{
    m_pageSizef = QSizeF(page->width(), page->height());
}

PDFPage::~PDFPage()
{
    LOCK_PAGE

    delete m_page;

    m_page = nullptr;
}

QSizeF PDFPage::sizeF() const
{
    return m_pageSizef;
}

QImage PDFPage::render(const qreal &scaleFactor) const
{
    LOCK_PAGE

    if (m_page == nullptr)
        return QImage();

    return m_page->image(scaleFactor);
}

QImage PDFPage::render(Dr::Rotation rotation, const double scaleFactor, const QRectF &boundingRect) const
{
    return render(scaleFactor, scaleFactor, rotation, boundingRect);
}

QImage PDFPage::render(int width, int height, Qt::AspectRatioMode) const
{
    qreal horizontalResolution = 1.0 * width / m_page->width();
    qreal verticalResolution = 1.0 * height / m_page->height();

    return render(horizontalResolution, verticalResolution, Dr::RotateBy0, QRect(0, 0, width, height));
}

QImage PDFPage::render(qreal horizontalResolution, qreal verticalResolution, Dr::Rotation, QRectF boundingRect) const
{
    LOCK_PAGE

    if (m_page == nullptr)
        return QImage();

    qreal x = 0;
    qreal y = 0;
    qreal w = m_page->width();
    qreal h = m_page->height();

    if (!boundingRect.isNull()) {
        x = boundingRect.x();
        y = boundingRect.y();
        w = boundingRect.width();
        h = boundingRect.height();
    }

    return m_page->image(horizontalResolution, verticalResolution, x, y, w, h);
}

Link PDFPage::getLinkAtPoint(const QPointF &point) const
{
    Link link;
    DPdfPage::Link dlInk = m_page->getLinkAtPoint(point.x(), point.y());
    if (dlInk.isValid()) {
        link.page = dlInk.nIndex + 1;
        link.urlOrFileName = dlInk.urlpath;
        link.left = dlInk.left;
        link.top = dlInk.top;
    }

    return link;
}

QString PDFPage::text(const QRectF &rect) const
{
    return m_page->text(rect).simplified();
}

QList<Word> PDFPage::words()
{
    if (m_words.size() > 0)
        return m_words;

    int charCount = m_page->countChars();
    QPointF lastOffset(0, 0);

    for (int i = 0; i < charCount; i++) {
        Word word;
        word.text = m_page->text(i);

        QRectF rectf;
        if (m_page->getTextRect(i, rectf)) {
            //换行
            if (rectf.top() > lastOffset.y() || lastOffset.y() > rectf.bottom())
                lastOffset = QPointF(rectf.x(), rectf.center().y());

            if (rectf.width() > 0) {
                word.boundingBox = QRectF(lastOffset.x(), rectf.y(), rectf.width() + rectf.x() - lastOffset.x(), rectf.height());
                lastOffset = QPointF(word.boundingBox.right(), word.boundingBox.center().y());

                int curWordsSize = m_words.size();
                if (curWordsSize > 2 && static_cast<int>(m_words.at(curWordsSize - 1).wordBoundingRect().width()) == 0
                        && m_words.at(curWordsSize - 2).wordBoundingRect().width() > 0
                        && word.boundingBox.center().y() <= m_words.at(curWordsSize - 2).wordBoundingRect().bottom()
                        && word.boundingBox.center().y() >= m_words.at(curWordsSize - 2).wordBoundingRect().top()) {
                    QRectF prevBoundRectF = m_words.at(curWordsSize - 2).wordBoundingRect();
                    m_words[curWordsSize - 1].boundingBox = QRectF(prevBoundRectF.right(), prevBoundRectF.y(), word.boundingBox.x() - prevBoundRectF.right(), prevBoundRectF.height());
                }
            }

            m_words.append(word);
        }
    }
    return m_words;
}

QVector<QRectF> PDFPage::search(const QString &text, bool matchCase, bool wholeWords) const
{
    QVector<QRectF> results;

    results = m_page->search(text, matchCase, wholeWords);

    return results;
}

QList< Annotation * > PDFPage::annotations() const
{
    QList< Annotation * > annotations;

    foreach (DPdfAnnot *annotation, m_page->annots()) {
        if (annotation->type() == DPdfAnnot::AText || annotation->type() == DPdfAnnot::AHighlight) {
            annotations.append(new PDFAnnotation(m_mutex, annotation));
            continue;
        }

        delete annotation;
    }

    return annotations;
}

Annotation *PDFPage::addHighlightAnnotation(const QList<QRectF> &boundarys, const QString &text, const QColor &color)
{
    return new PDFAnnotation(m_mutex, m_page->createHightLightAnnot(boundarys, text, color));
}

bool PDFPage::removeAnnotation(deepin_reader::Annotation *annotation)
{
    deepin_reader::PDFAnnotation *PDFAnnotation = static_cast< deepin_reader::PDFAnnotation * >(annotation);

    if (PDFAnnotation == nullptr)
        return false;

    m_page->removeAnnot(PDFAnnotation->m_annotation);

    PDFAnnotation->m_annotation = nullptr;
    delete PDFAnnotation;
    PDFAnnotation = nullptr;

    return true;
}

bool PDFPage::updateAnnotation(Annotation *annotation, const QString &text, const QColor &color)
{
    if (nullptr == annotation)
        return false;

    if (m_page->annots().contains(annotation->ownAnnotation())) {
        if (annotation->type() == DPdfAnnot::AText)
            m_page->updateTextAnnot(annotation->ownAnnotation(), text);
        else
            m_page->updateHightLightAnnot(annotation->ownAnnotation(), color, text);
        return true;
    }

    return false;
}

bool PDFPage::mouseClickIconAnnot(QPointF &clickPoint)
{
    foreach (DPdfAnnot *annot, m_page->annots()) {
        if (annot && annot->pointIn(clickPoint)) {
            return true;
        }
    }

    return false;
}

Annotation *PDFPage::addIconAnnotation(const QRectF &rect, const QString &text)
{
    if (nullptr == m_page)
        return nullptr;

    return new PDFAnnotation(m_mutex, m_page->createTextAnnot(rect.center(), text));
}

Annotation *PDFPage::moveIconAnnotation(Annotation *annot, const QRectF &rect)
{
    if (nullptr == m_page || nullptr == annot)
        return nullptr;

    if (annot->ownAnnotation()) {
        m_page->updateTextAnnot(annot->ownAnnotation(), annot->ownAnnotation()->text(), rect.center());
        return annot;
    }
    return nullptr;
}

PDFDocument::PDFDocument(DPdfDoc *document) :
    m_mutex(),
    m_document(document)
{
}

PDFDocument::~PDFDocument()
{
    delete m_document;
    m_document = nullptr;
}

int PDFDocument::numberOfPages() const
{
    return m_document->pageCount();
}

Page *PDFDocument::page(int index) const
{
    if (DPdfPage *page = m_document->page(index)) {
        return new PDFPage(&m_mutex, page);
    }

    return nullptr;
}

QString PDFDocument::label(int index) const
{
    return m_document->label(index);
}

QSizeF PDFDocument::pageSizeF(int index) const
{
    return m_document->pageSizeF(index);
}

QStringList PDFDocument::saveFilter() const
{
    return QStringList() << "Portable document format (*.pdf)";
}

bool PDFDocument::save(const QString &filePath) const
{
    LOCK_DOCUMENT
    Q_UNUSED(filePath)

    return m_document->save();
}

bool PDFDocument::saveAs(const QString &filePath) const
{
    LOCK_DOCUMENT
    return m_document->saveAs(filePath);
}

void collectOuleLine(const DPdfDoc::Outline &cOutline, deepin_reader::Outline &outline)
{
    for (const DPdfDoc::Section &cSection : cOutline) {
        deepin_reader::Section setction;
        setction.nIndex = cSection.nIndex;
        setction.title = cSection.title;
        setction.offsetPointF = cSection.offsetPointF;
        if (cSection.children.size() > 0) {
            collectOuleLine(cSection.children, setction.children);
        }
        outline << setction;
    }
}

Outline PDFDocument::outline() const
{
    LOCK_DOCUMENT

    if (m_outline.size() > 0)
        return m_outline;

    const DPdfDoc::Outline &cOutline = m_document->outline();
    collectOuleLine(cOutline, m_outline);
    return m_outline;
}

Properties PDFDocument::properties() const
{
    LOCK_DOCUMENT

    if (m_fileProperties.size() > 0)
        return m_fileProperties;

    m_fileProperties = m_document->proeries();

    return m_fileProperties;
}

PDFDocument *PDFDocument::loadDocument(const QString &filePath, const QString &password, int &status)
{
    status = DPdfDoc::tryLoadFile(filePath, password);
    if (status == DPdfDoc::SUCCESS) {
        DPdfDoc *document = new DPdfDoc(filePath, password);
        return new deepin_reader::PDFDocument(document);
    }
    return nullptr;
}

}


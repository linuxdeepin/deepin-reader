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
#include "dpdfannot.h"
#include "dpdfpage.h"
#include "dpdfdoc.h"
#include "Application.h"

#include <QDebug>
#include <QScreen>

#define LOCK_APPPAGE  QMutexLocker docMutexLocker(&PDFPage::s_mutex);
#define LOCK_DOCUMENT QMutexLocker docMutexLocker(m_docMutex);

namespace deepin_reader {

PDFAnnotation::PDFAnnotation(DPdfAnnot *dannotation) : Annotation(),
    m_dannotation(dannotation)
{
}

PDFAnnotation::~PDFAnnotation()
{
    m_dannotation = nullptr;
}

QList<QRectF> PDFAnnotation::boundary() const
{
    QList<QRectF> rectFList;

    if (m_dannotation->type() == DPdfAnnot::AText || m_dannotation->type() == DPdfAnnot::AHighlight) {

        foreach (QRectF rect, m_dannotation->boundaries())
            rectFList.append(rect);
    }

    return rectFList;
}

QString PDFAnnotation::contents() const
{
    if (nullptr == m_dannotation)
        return QString();

    return m_dannotation->text();
}

int PDFAnnotation::type()
{
    if (nullptr == m_dannotation)
        return -1;

    return m_dannotation->type();
}

DPdfAnnot *PDFAnnotation::ownAnnotation()
{
    return m_dannotation;
}

QMutex PDFPage::s_mutex;
PDFPage::PDFPage(QMutex *mutex, DPdfPage *page) :
    m_docMutex(mutex), m_page(page)
{

}

PDFPage::~PDFPage()
{

}

QSizeF PDFPage::sizeF() const
{
    return m_page->sizeF();
}

QImage PDFPage::render(int width, int height, const QRect &slice) const
{
    LOCK_DOCUMENT

    QRect ratioRect = slice.isValid() ? QRect(static_cast<int>(slice.x() * dApp->devicePixelRatio()),
                                              static_cast<int>(slice.y() * dApp->devicePixelRatio()),
                                              static_cast<int>(slice.width() * dApp->devicePixelRatio()),
                                              static_cast<int>(slice.height() * dApp->devicePixelRatio())) : QRect();

    QImage image = m_page->image(static_cast<int>(width * dApp->devicePixelRatio()),
                                 static_cast<int>(height * dApp->devicePixelRatio()), ratioRect);

    image.setDevicePixelRatio(dApp->devicePixelRatio());

    return image;
}

Link PDFPage::getLinkAtPoint(const QPointF &pos)
{
    Link link;
    const QList<DPdfAnnot *> &dlinkAnnots = m_page->links();
    if (dlinkAnnots.size() > 0) {
        for (DPdfAnnot *annot : dlinkAnnots) {
            DPdfLinkAnnot *linkAnnot = dynamic_cast<DPdfLinkAnnot *>(annot);
            if (linkAnnot && linkAnnot->pointIn(pos)) {
                link.page = linkAnnot->pageIndex() + 1;
                link.urlOrFileName = linkAnnot->url().isEmpty() ? linkAnnot->filePath() : linkAnnot->url();
                link.left = linkAnnot->offset().x();
                link.top = linkAnnot->offset().y();
                return link;
            }
        }
    }
    return link;
}

QString PDFPage::text(const QRectF &rect) const
{
    return m_page->text(rect).simplified();
}

QList<Word> PDFPage::words()
{
    LOCK_DOCUMENT

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
                if (curWordsSize > 2
                        && static_cast<int>(m_words.at(curWordsSize - 1).wordBoundingRect().width()) == 0
                        && m_words.at(curWordsSize - 2).wordBoundingRect().width() > 0
                        && word.boundingBox.center().y() <= m_words.at(curWordsSize - 2).wordBoundingRect().bottom()
                        && word.boundingBox.center().y() >= m_words.at(curWordsSize - 2).wordBoundingRect().top()) {
                    QRectF prevBoundRectF = m_words.at(curWordsSize - 2).wordBoundingRect();
                    m_words[curWordsSize - 1].boundingBox = QRectF(prevBoundRectF.right(),
                                                                   prevBoundRectF.y(),
                                                                   word.boundingBox.x() - prevBoundRectF.right(),
                                                                   prevBoundRectF.height());
                }
            }

            m_words.append(word);
        }
    }
    return m_words;
}

QVector<QRectF> PDFPage::search(const QString &text, bool matchCase, bool wholeWords) const
{
    LOCK_DOCUMENT

    QVector<QRectF> results;

    results = m_page->search(text, matchCase, wholeWords);

    return results;
}

QList< Annotation * > PDFPage::annotations() const
{
    QList< Annotation * > annotations;

    foreach (DPdfAnnot *dannotation, m_page->annots()) {
        annotations.append(new PDFAnnotation(dannotation));
    }

    return annotations;
}

Annotation *PDFPage::addHighlightAnnotation(const QList<QRectF> &boundaries, const QString &text, const QColor &color)
{
    LOCK_DOCUMENT

    return new PDFAnnotation(m_page->createHightLightAnnot(boundaries, text, color));
}

bool PDFPage::removeAnnotation(deepin_reader::Annotation *annotation)
{
    LOCK_DOCUMENT

    deepin_reader::PDFAnnotation *PDFAnnotation = static_cast< deepin_reader::PDFAnnotation * >(annotation);

    if (PDFAnnotation == nullptr)
        return false;

    m_page->removeAnnot(PDFAnnotation->m_dannotation);

    PDFAnnotation->m_dannotation = nullptr;

    delete PDFAnnotation;

    return true;
}

bool PDFPage::updateAnnotation(Annotation *annotation, const QString &text, const QColor &color)
{
    LOCK_DOCUMENT

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

bool PDFPage::mouseClickIconAnnot(const QPointF &clickPos)
{
    LOCK_DOCUMENT

    foreach (DPdfAnnot *annot, m_page->annots()) {
        if (annot && annot->pointIn(clickPos)) {
            return true;
        }
    }

    return false;
}

Annotation *PDFPage::addIconAnnotation(const QRectF &rect, const QString &text)
{
    LOCK_DOCUMENT

    if (nullptr == m_page)
        return nullptr;

    return new PDFAnnotation(m_page->createTextAnnot(rect.center(), text));
}

Annotation *PDFPage::moveIconAnnotation(Annotation *annot, const QRectF &rect)
{
    LOCK_DOCUMENT

    if (nullptr == m_page || nullptr == annot)
        return nullptr;

    if (annot->ownAnnotation()) {
        m_page->updateTextAnnot(annot->ownAnnotation(), annot->ownAnnotation()->text(), rect.center());

        return annot;
    }
    return nullptr;
}

PDFDocument::PDFDocument(DPdfDoc *document) :
    m_document(document)
{
    m_docMutex = new QMutex;

    QScreen *srn = QApplication::screens().value(0);
    if (nullptr != srn) {
        m_xRes = srn->physicalDotsPerInchX();
        m_yRes = srn->physicalDotsPerInchY();
    }
}

PDFDocument::~PDFDocument()
{
    //需要确保pages先被析构完成

    m_docMutex->lock();

    delete m_document;

    m_document = nullptr;

    m_docMutex->unlock();

    delete m_docMutex;
}

int PDFDocument::pageCount() const
{
    LOCK_DOCUMENT

    return m_document->pageCount();
}

Page *PDFDocument::page(int index) const
{
    LOCK_DOCUMENT

    if (DPdfPage *page = m_document->page(index, m_xRes, m_yRes)) {
        if (page->isValid())
            return new PDFPage(m_docMutex, page);
    }

    return nullptr;
}

QString PDFDocument::label(int index) const
{
    LOCK_DOCUMENT

    return m_document->label(index);
}

QStringList PDFDocument::saveFilter() const
{
    return QStringList() << "Portable document format (*.pdf)";
}

bool PDFDocument::save() const
{
    LOCK_DOCUMENT

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

    const DPdfDoc::Outline &cOutline = m_document->outline(m_xRes, m_yRes);

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

PDFDocument *PDFDocument::loadDocument(const QString &filePath, const QString &password)
{
    DPdfDoc *document = new DPdfDoc(filePath, password);
    if (document->status() == DPdfDoc::SUCCESS)
        return new deepin_reader::PDFDocument(document);
    else {
        delete document;
        return nullptr;
    }
}

int PDFDocument::tryLoadDocument(const QString &filePath, const QString &password)
{
    return DPdfDoc::tryLoadFile(filePath, password);
}

}


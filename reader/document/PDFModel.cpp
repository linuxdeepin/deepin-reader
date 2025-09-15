// SPDX-FileCopyrightText: 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "PDFModel.h"
#include "dpdfannot.h"
#include "dpdfpage.h"
#include "dpdfdoc.h"
#include "Application.h"

#include <QDebug>
#include <QScreen>
#include <QThread>

#include <functional>

#define LOCK_DOCUMENT QMutexLocker docMutexLocker(m_docMutex);

namespace deepin_reader {
PDFAnnotation::PDFAnnotation(DPdfAnnot *dannotation) : Annotation(),
    m_dannotation(dannotation)
{
    // qDebug() << "Creating PDF annotation with type:" << (dannotation ? dannotation->type() : -1);
}

PDFAnnotation::~PDFAnnotation()
{
    // qDebug() << "Destroying PDF annotation";

    m_dannotation = nullptr;
}

QList<QRectF> PDFAnnotation::boundary() const
{
    qDebug() << "PDFAnnotation::boundary() - Getting annotation boundary";
    QList<QRectF> rectFList;

    if (m_dannotation->type() == DPdfAnnot::AText || m_dannotation->type() == DPdfAnnot::AHighlight) {
        qDebug() << "PDFAnnotation::boundary() - Processing text or highlight annotation";
        foreach (QRectF rect, m_dannotation->boundaries())
            rectFList.append(rect);
    }

    qDebug() << "PDFAnnotation::boundary() - Returning" << rectFList.size() << "boundaries";
    return rectFList;
}

QString PDFAnnotation::contents() const
{
    qDebug() << "PDFAnnotation::contents() - Getting annotation contents";
    if (nullptr == m_dannotation) {
        qWarning() << "Attempt to get contents from null annotation";
        return QString();
    }

    QString text = m_dannotation->text();
    qDebug() << "PDFAnnotation::contents() - Returning contents:" << text;
    return text;
}

int PDFAnnotation::type()
{
    qDebug() << "PDFAnnotation::type() - Getting annotation type";
    if (nullptr == m_dannotation) {
        qWarning() << "Attempt to get type from null annotation";
        return -1;
    }

    int type = m_dannotation->type();
    qDebug() << "PDFAnnotation::type() - Returning type:" << type;
    return type;
}

DPdfAnnot *PDFAnnotation::ownAnnotation()
{
    // qDebug() << "PDFAnnotation::ownAnnotation() - Getting own annotation pointer";
    return m_dannotation;
}

PDFPage::PDFPage(QMutex *mutex, DPdfPage *page) :
    m_docMutex(mutex), m_page(page)
{
    // qInfo() << "Creating PDF page handler";
}

PDFPage::~PDFPage()
{
    // qInfo() << "Destroying PDF page handler";
}

QSizeF PDFPage::sizeF() const
{
    // qDebug() << "PDFPage::sizeF() - Getting page size";
    QSizeF size = m_page->sizeF();
    // qDebug() << "PDFPage::sizeF() - Returning size:" << size;
    return size;
}

QImage PDFPage::render(int width, int height, const QRect &slice) const
{
    // qInfo() << "Rendering PDF page with size:" << width << "x" << height << "slice:" << slice;

    LOCK_DOCUMENT

    QRect ratioRect = slice.isValid() ? QRect(slice.x(), slice.y(), slice.width(), slice.height()) : QRect();

    QImage result = m_page->image(width, height, ratioRect);
    // qDebug() << "PDFPage::render() - Render completed";
    return result;
}

Link PDFPage::getLinkAtPoint(const QPointF &pos)
{
    // qDebug() << "PDFPage::getLinkAtPoint() - Getting link at point:" << pos;
    Link link;
    const QList<DPdfAnnot *> &dlinkAnnots = m_page->links();
    if (dlinkAnnots.size() > 0) {
        // qDebug() << "PDFPage::getLinkAtPoint() - Found" << dlinkAnnots.size() << "link annotations";
        for (DPdfAnnot *annot : dlinkAnnots) {
            DPdfLinkAnnot *linkAnnot = dynamic_cast<DPdfLinkAnnot *>(annot);
            if (linkAnnot && linkAnnot->pointIn(pos)) {
                // qDebug() << "PDFPage::getLinkAtPoint() - Found matching link annotation";
                if (!linkAnnot->isValid())
                    m_page->initAnnot(annot);

                link.page = linkAnnot->pageIndex() + 1;
                if (linkAnnot->url().isEmpty()) {
                    link.urlOrFileName = linkAnnot->filePath();
                } else {
                    // 删除邮件或本地文件的前缀"http://"
                    link.urlOrFileName = (linkAnnot->url().startsWith(QLatin1String("http://mailto:"))
                                          || linkAnnot->url().startsWith(QLatin1String("http://file://")))
                                         ? linkAnnot->url().mid(7) : linkAnnot->url();
                }
                link.left = linkAnnot->offset().x();
                link.top = linkAnnot->offset().y();
                return link;
            }
        }
    }
    // qDebug() << "PDFPage::getLinkAtPoint() - No matching link found";
    return link;
}

bool PDFPage::hasWidgetAnnots() const
{
    // qDebug() << "Checking if page has widget annotations";
    return m_page->widgets().count() > 0;
}

QString PDFPage::text(const QRectF &rect) const
{
    // qDebug() << "PDFPage::text() - Getting text from rect:" << rect;
    return m_page->text(rect).simplified();
}

QList<Word> PDFPage::words()
{
    qInfo() << "Extracting text words from PDF page";

    LOCK_DOCUMENT

    if (m_wordLoaded) {
        qDebug() << "PDFPage::words() - Words already loaded, returning cached words";
        return m_words;
    }

    int charCount = 0;

    QStringList texts;

    QVector<QRectF> rects;

    m_page->allTextLooseRects(charCount, texts, rects);

    m_wordLoaded = true;

    if (charCount <= 0) {
        qDebug() << "PDFPage::words() - No characters found, returning empty word list";
        return m_words;
    }

    int index = 0;

    qreal maxY = rects[0].y();

    qreal maxHeight = rects[0].height();

    QList<Word> rowWords;

    while (1) {
        if (index == charCount) {
            // qDebug() << "PDFPage::words() - Processed all characters, appending final row";
            m_words.append(rowWords);
            break;
        }

        Word word;
        word.text = texts[index];
        word.boundingBox = rects[index];

        //不进行优化
//        m_words.append(word);
//        ++index;
//        continue;

        //如果重叠的部分只在三分之一，视为换行
        if ((word.boundingBox.y() > maxY + maxHeight - (word.boundingBox.height() / 3)) ||
                (word.boundingBox.y() + word.boundingBox.height() < maxY  + (word.boundingBox.height() / 3))) {

            //最本行所有文字 补齐高和低不符合的 填满宽度
            for (int i = 0; i < rowWords.count(); ++i) {
                rowWords[i].boundingBox.setY(maxY);
                rowWords[i].boundingBox.setHeight(maxHeight);

                //如果是空格类型 且没有宽度
                if (!rowWords[i].text.isEmpty() && rowWords[i].text.trimmed().isEmpty() && rowWords[i].boundingBox.width() < 0.0001) {
                    continue;
                    //                if (i - 1 >= 0 && i + 1 < rowWords.count()) {
                    //                    if (rowWords[i + 1].boundingBox.x() - (rowWords[i - 1].boundingBox.x() + rowWords[i - 1].boundingBox.width()) < 10) {
                    //                        rowWords[i].boundingBox.setY(maxY);
                    //                        rowWords[i].boundingBox.setX(rowWords[i - 1].boundingBox.x() + rowWords[i - 1].boundingBox.width());
                    //                        rowWords[i].boundingBox.setWidth(rowWords[i + 1].boundingBox.x() - rowWords[i - 1].boundingBox.x() - rowWords[i - 1].boundingBox.width());
                    //                    }
                    //                }
                }

                //找出本行下一个非空的索引
                int nextIndex = i + 1;
                while (nextIndex < rowWords.count()) {
                    if (!rowWords[nextIndex].text.trimmed().isEmpty() || rowWords[nextIndex].boundingBox.width() > 0.0001)
                        break;
                    nextIndex++;
                }

                //将本文字宽度拉长到下一个字/如果和下一个x重合，则稍微减少点自己宽度
                if (((nextIndex) < rowWords.count()) && !rowWords[nextIndex].text.trimmed().isEmpty() &&
                        (((rowWords[i].boundingBox.x() + rowWords[i].boundingBox.width() < rowWords[nextIndex].boundingBox.x()) &&
                          (rowWords[nextIndex].boundingBox.x() - (rowWords[i].boundingBox.x() + rowWords[i].boundingBox.width()) < 10)) ||
                         ((rowWords[i].boundingBox.x() + rowWords[i].boundingBox.width() > rowWords[nextIndex].boundingBox.x())))
                   ) {
                    rowWords[i].boundingBox.setWidth(rowWords[nextIndex].boundingBox.x() - rowWords[i].boundingBox.x());
                }
            }

            m_words.append(rowWords);
            rowWords.clear();
            maxY = word.boundingBox.y();
            maxHeight = word.boundingBox.height();
        }

        //如果上一个是空格 这一个还是空 则只是追加
//        if (!rowWords.isEmpty() && rowWords[rowWords.count() - 1].text.trimmed().isEmpty() && word.text.trimmed().isEmpty()) {
//            rowWords[rowWords.count() - 1].text.append(word.text);
//            ++index;
//            continue;
//        }

        //先取出最高和最低
        if (word.boundingBox.y() < maxY)
            maxY = word.boundingBox.y();

        if (word.boundingBox.height() + (word.boundingBox.y() - maxY) > maxHeight)
            maxHeight = word.boundingBox.height() + (word.boundingBox.y() - maxY);

        rowWords.append(word);

        ++index;
    }

//    for (int i = 0; i < charCount; i++) {
//        if (texts.count() <= i || rects.count() <= i)
//            break;

//        Word word;
//        word.text = texts[i];
//        QRectF rectf = rects[i];

//        //换行
//        if (rectf.top() > lastOffset.y() || lastOffset.y() > rectf.bottom())
//            lastOffset = QPointF(rectf.x(), rectf.center().y());

//        if (rectf.width() > 0) {
//            word.boundingBox = QRectF(lastOffset.x(), rectf.y(), rectf.width() + rectf.x() - lastOffset.x(), rectf.height());

//            lastOffset = QPointF(word.boundingBox.right(), word.boundingBox.center().y());

//            int curWordsSize = m_words.size();
//            if (curWordsSize > 2
//                    && static_cast<int>(m_words.at(curWordsSize - 1).wordBoundingRect().width()) == 0
//                    && m_words.at(curWordsSize - 2).wordBoundingRect().width() > 0
//                    && word.boundingBox.center().y() <= m_words.at(curWordsSize - 2).wordBoundingRect().bottom()
//                    && word.boundingBox.center().y() >= m_words.at(curWordsSize - 2).wordBoundingRect().top()) {
//                QRectF prevBoundRectF = m_words.at(curWordsSize - 2).wordBoundingRect();
//                m_words[curWordsSize - 1].boundingBox = QRectF(prevBoundRectF.right(),
//                                                               prevBoundRectF.y(),
//                                                               word.boundingBox.x() - prevBoundRectF.right(),
//                                                               prevBoundRectF.height());
//            }
//        }

//        m_words.append(word);
//    }

    qDebug() << "PDFPage::words() - Word extraction completed, total words:" << m_words.size();
    return m_words;
}

QVector<PageSection> PDFPage::search(const QString &text, bool matchCase, bool wholeWords) const
{
    qInfo() << "Searching PDF page for text:" << text << "matchCase:" << matchCase << "wholeWords:" << wholeWords;

    LOCK_DOCUMENT

    return m_page->search(text, matchCase, wholeWords);
}

QList< Annotation * > PDFPage::annotations() const
{
    qDebug() << "Retrieving PDF page annotations";

    QList< Annotation * > annotations;

    const QList<DPdfAnnot *> &annos = m_page->annots();

    foreach (DPdfAnnot *dannotation, annos) {
        annotations.append(new PDFAnnotation(dannotation));
    }

    // qDebug() << "PDFPage::annotations() - Found" << annotations.size() << "annotations";
    return annotations;
}

Annotation *PDFPage::addHighlightAnnotation(const QList<QRectF> &boundaries, const QString &text, const QColor &color)
{
    qInfo() << "Adding highlight annotation with boundaries:" << boundaries << "text:" << text << "color:" << color;

    return new PDFAnnotation(m_page->createHightLightAnnot(boundaries, text, color));
}

bool PDFPage::removeAnnotation(Annotation *annotation)
{
    qInfo() << "Removing PDF annotation";

    PDFAnnotation *PDFAnnotation = static_cast< class PDFAnnotation * >(annotation);

    if (PDFAnnotation == nullptr) {
        qWarning() << "Attempt to remove null annotation";
        return false;
    }

    m_page->removeAnnot(PDFAnnotation->m_dannotation);

    PDFAnnotation->m_dannotation = nullptr;

    delete PDFAnnotation;

    qDebug() << "PDFPage::removeAnnotation() - Successfully removed annotation";
    return true;
}

bool PDFPage::updateAnnotation(Annotation *annotation, const QString &text, const QColor &color)
{
    qInfo() << "Updating PDF annotation with text:" << text << "color:" << color;

    if (nullptr == annotation) {
        qWarning() << "Attempt to update null annotation";
        return false;
    }

    if (m_page->annots().contains(annotation->ownAnnotation())) {
        qDebug() << "PDFPage::updateAnnotation() - Found annotation, updating";
        if (annotation->type() == DPdfAnnot::AText) {
            qDebug() << "PDFPage::updateAnnotation() - Updating text annotation";
            m_page->updateTextAnnot(annotation->ownAnnotation(), text);
        } else {
            qDebug() << "PDFPage::updateAnnotation() - Updating highlight annotation";
            m_page->updateHightLightAnnot(annotation->ownAnnotation(), color, text);
        }
        return true;
    }

    qDebug() << "PDFPage::updateAnnotation() - Failed, annotation not found";
    return false;
}

Annotation *PDFPage::addIconAnnotation(const QRectF &rect, const QString &text)
{
    qInfo() << "Adding icon annotation at rect:" << rect << "with text:" << text;

    if (nullptr == m_page) {
        qCritical() << "Attempt to add icon annotation to null page";
        return nullptr;
    }

    return new PDFAnnotation(m_page->createTextAnnot(rect.center(), text));
}

Annotation *PDFPage::moveIconAnnotation(Annotation *annot, const QRectF &rect)
{
    qInfo() << "Moving icon annotation to rect:" << rect;

    if (nullptr == m_page || nullptr == annot) {
        qCritical() << "Attempt to move icon annotation with null page or annotation";
        return nullptr;
    }

    if (annot->ownAnnotation()) {
        qDebug() << "PDFPage::moveIconAnnotation() - Moving annotation";
        m_page->updateTextAnnot(annot->ownAnnotation(), annot->ownAnnotation()->text(), rect.center());

        return annot;
    }
    qDebug() << "PDFPage::moveIconAnnotation() - Failed, no annotation data";
    return nullptr;
}

PDFDocument::PDFDocument(DPdfDoc *document) :
    m_document(document)
{
    qInfo() << "Creating PDF document handler with page count:" << (document ? document->pageCount() : 0);

    m_docMutex = new QMutex;

    QScreen *srn = QApplication::screens().value(0);
    if (nullptr != srn) {
        qDebug() << "PDFDocument::PDFDocument() - Screen DPI set, x:" << m_xRes << "y:" << m_yRes;
        m_xRes = srn->logicalDotsPerInchX(); // 获取屏幕的横纵向逻辑dpi
        m_yRes = srn->logicalDotsPerInchY();
    }
    qDebug() << "PDFDocument::PDFDocument() - Constructor completed";
}

PDFDocument::~PDFDocument()
{
    qInfo() << "Destroying PDF document handler";

    //需要确保pages先被析构完成
    qDebug() << "Releasing PDF document resources...";

    m_docMutex->lock();

    delete m_document;

    m_document = nullptr;

    m_docMutex->unlock();

    delete m_docMutex;
    qDebug() << "PDF document resources released";
}

int PDFDocument::pageCount() const
{
    qDebug() << "Retrieving PDF document page count";

    return m_document->pageCount();
}

Page *PDFDocument::page(int index) const
{
    qInfo() << "Retrieving PDF page at index:" << index;

    if (DPdfPage *page = m_document->page(index, m_xRes, m_yRes)) {
        qDebug() << "PDFDocument::page() - Page object created";
        if (page->isValid()) {
            qDebug() << "PDF page at index" << index << "is valid";
            return new PDFPage(m_docMutex, page);
        }
        qWarning() << "PDF page at index" << index << "is invalid";
    }

    qDebug() << "PDFDocument::page() - Returning null page";
    return nullptr;
}

QString PDFDocument::label(int index) const
{
    qDebug() << "Retrieving label for PDF page at index:" << index;

    return m_document->label(index);
}

QStringList PDFDocument::saveFilter() const
{
    qDebug() << "Retrieving PDF save filters";

    return QStringList() << "Portable document format (*.pdf)";
}

bool PDFDocument::save() const
{
    qInfo() << "Saving PDF document";

    return m_document->save();
}

bool PDFDocument::saveAs(const QString &filePath) const
{
    qInfo() << "Saving PDF document as:" << filePath;

    return m_document->saveAs(filePath);
}

void collectOuleLine(const DPdfDoc::Outline &cOutline, Outline &outline)
{
    qDebug() << "collectOuleLine() - Processing outline with" << cOutline.size() << "sections";
    for (const DPdfDoc::Section &cSection : cOutline) {
        Section setction;
        setction.nIndex = cSection.nIndex;
        setction.title = cSection.title;
        setction.offsetPointF = cSection.offsetPointF;
        if (cSection.children.size() > 0) {
            // qDebug() << "collectOuleLine() - Processing" << cSection.children.size() << "child sections";
            collectOuleLine(cSection.children, setction.children);
        }
        outline << setction;
    }
    qDebug() << "collectOuleLine() - Completed processing outline";
}

Outline PDFDocument::outline() const
{
    qInfo() << "Retrieving PDF document outline";

    if (m_outline.size() > 0) {
        qDebug() << "Returning cached PDF outline";
        return m_outline;
    }

    const DPdfDoc::Outline &cOutline = m_document->outline(m_xRes, m_yRes);

    collectOuleLine(cOutline, m_outline);

    qDebug() << "PDFDocument::outline() - Outline built with" << m_outline.size() << "sections";
    return m_outline;
}

Properties PDFDocument::properties() const
{
    qInfo() << "Retrieving PDF document properties";

    if (m_fileProperties.size() > 0) {
        qDebug() << "Returning cached PDF properties";
        return m_fileProperties;
    }

    m_fileProperties = m_document->proeries();

    qDebug() << "PDFDocument::properties() - Properties loaded, count:" << m_fileProperties.size();
    return m_fileProperties;
}

PDFDocument *PDFDocument::loadDocument(const QString &filePath, const QString &password, deepin_reader::Document::Error &error)
{
    qInfo() << "Loading PDF document from:" << filePath << "with password:" << (!password.isEmpty() ? "[provided]" : "[not provided]");

    DPdfDoc *document = new DPdfDoc(filePath, password);

    if (document->status() == DPdfDoc::SUCCESS) {
        qInfo() << "PDF document loaded successfully";
        error = Document::NoError;
        return new PDFDocument(document);
    } else if (document->status() == DPdfDoc::PASSWORD_ERROR) {
        qDebug() << "PDFDocument::loadDocument() - Document status: PASSWORD_ERROR";
        if (password.isEmpty()) {
            qWarning() << "PDF document requires password but none provided";
            error = Document::NeedPassword;
        } else {
            qWarning() << "Incorrect password provided for PDF document";
            error = Document::WrongPassword;
        }
    } else {
        qCritical() << "Failed to load PDF document, error code:" << document->status();
        error = Document::FileError;
    }

    delete document;
    qDebug() << "PDFDocument::loadDocument() - Returning null document";
    return nullptr;
}
}


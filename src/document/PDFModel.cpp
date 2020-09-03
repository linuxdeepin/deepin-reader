/*

Copyright 2014 S. Razi Alavizadeh
Copyright 2018 Marshall Banana
Copyright 2013-2014, 2018 Adam Reichold

This file is part of qpdfview.

qpdfview is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

qpdfview is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with qpdfview.  If not, see <http://www.gnu.org/licenses/>.

*/
#include "PDFModel.h"

#include <QCache>
#include <QFormLayout>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>

#if QT_VERSION >= QT_VERSION_CHECK(5,0,0)

#include <poppler-qt5.h>

#else

#include <poppler-qt4.h>

#endif // QT_VERSION
#include <QDebug>
#include <QTimer>

#include <poppler-form.h>

#ifndef HAS_POPPLER_24

#define LOCK_ANNOTATION QMutexLocker mutexLocker(m_mutex);
#define LOCK_FORM_FIELD QMutexLocker mutexLocker(m_mutex);
#define LOCK_PAGE QMutexLocker mutexLocker(m_mutex);
#define LOCK_DOCUMENT QMutexLocker mutexLocker(&m_mutex);

#else

#define LOCK_ANNOTATION
#define LOCK_FORM_FIELD
#define LOCK_PAGE
#define LOCK_DOCUMENT
#define LOCK_LONGTIMEOPERATION QMutexLocker mutexLocker(m_mutex);

#endif // HAS_POPPLER_24

namespace {

using namespace deepin_reader;

Outline loadOutline(const QDomNode &parent, Poppler::Document *document)
{
    Outline outline;

    const QDomNodeList nodes = parent.childNodes();

    outline.reserve(nodes.size());

    for (int index = 0, count = nodes.size(); index < count; ++index) {
        const QDomNode node = nodes.at(index);
        const QDomElement element = node.toElement();

        outline.push_back(Section());
        Section &section = outline.back();
        section.title = element.tagName();

        QScopedPointer< Poppler::LinkDestination > destination;

        if (element.hasAttribute("Destination")) {
            destination.reset(new Poppler::LinkDestination(element.attribute("Destination")));
        } else if (element.hasAttribute("DestinationName")) {
            destination.reset(document->linkDestination(element.attribute("DestinationName")));
        }

        if (destination) {
            int page = destination->pageNumber();
            qreal left = qQNaN();
            qreal top = qQNaN();

            page = page >= 1 ? page : 1;
            page = page <= document->numPages() ? page : document->numPages();

            if (destination->isChangeLeft()) {
                left = destination->left();

                left = left >= 0.0 ? left : 0.0;
                left = left <= 1.0 ? left : 1.0;
            }

            if (destination->isChangeTop()) {
                top = destination->top();

                top = top >= 0.0 ? top : 0.0;
                top = top <= 1.0 ? top : 1.0;
            }

            Link &link = section.link;
            link.page = page;
            link.left = left;
            link.top = top;

            const QString fileName = element.attribute("ExternalFileName");

            if (!fileName.isEmpty()) {
                link.urlOrFileName = fileName;
            }
        }

        if (node.hasChildNodes()) {
            section.children = loadOutline(node, document);
        }
    }

    return outline;
}

class FontsModel : public QAbstractTableModel
{
public:
    FontsModel(const QList< Poppler::FontInfo > &fonts) :
        m_fonts(fonts)
    {
    }

    int columnCount(const QModelIndex &) const
    {
        return 5;
    }

    int rowCount(const QModelIndex &parent) const
    {
        if (parent.isValid()) {
            return 0;
        }

        return m_fonts.size();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
            return QVariant();
        }

        switch (section) {
        case 0:
            return "Name";
        case 1:
            return "Type";
        case 2:
            return "Embedded";
        case 3:
            return "Subset";
        case 4:
            return "File";
        default:
            return QVariant();
        }
    }

    QVariant data(const QModelIndex &index, int role) const
    {
        if (!index.isValid() || role != Qt::DisplayRole) {
            return QVariant();
        }

        const Poppler::FontInfo &font = m_fonts[index.row()];

        switch (index.column()) {
        case 0:
            return font.name();
        case 1:
            return font.typeName();
        case 2:
            return font.isEmbedded() ? PDFDocument::tr("Yes") : PDFDocument::tr("No");
        case 3:
            return font.isSubset() ? PDFDocument::tr("Yes") : PDFDocument::tr("No");
        case 4:
            return font.file();
        default:
            return QVariant();
        }
    }

private:
    const QList< Poppler::FontInfo > m_fonts;

};

inline void restoreRenderHint(Poppler::Document *document, const Poppler::Document::RenderHints hints, const Poppler::Document::RenderHint hint)
{
    document->setRenderHint(hint, hints.testFlag(hint));
}

typedef QSharedPointer< Poppler::TextBox > TextBox;
typedef QList< TextBox > TextBoxList;

class TextCache
{
public:
    TextCache() : m_mutex(), m_cache(1 << 12) {}

    bool object(const PDFPage *page, TextBoxList &textBoxes) const
    {
        QMutexLocker mutexLocker(&m_mutex);

        if (TextBoxList *const object = m_cache.object(page)) {
            textBoxes = *object;

            return true;
        }

        return false;
    }

    void insert(const PDFPage *page, const TextBoxList &textBoxes)
    {
        QMutexLocker mutexLocker(&m_mutex);

        m_cache.insert(page, new TextBoxList(textBoxes), textBoxes.count());
    }

    void remove(const PDFPage *page)
    {
        QMutexLocker mutexLocker(&m_mutex);

        m_cache.remove(page);
    }

private:
    mutable QMutex m_mutex;
    QCache< const PDFPage *, TextBoxList > m_cache;

};

Q_GLOBAL_STATIC(TextCache, textCache)

namespace Defaults {

const bool antialiasing = true;
const bool textAntialiasing = true;

#ifdef HAS_POPPLER_18

const int textHinting = 0;

#else

const bool textHinting = false;

#endif // HAS_POPPLER_18

#ifdef HAS_POPPLER_35

const bool ignorePaperColor = false;

#endif // HAS_POPPLER_35

#ifdef HAS_POPPLER_22

const bool overprintPreview = false;

#endif // HAS_POPPLER_22

#ifdef HAS_POPPLER_24

const int thinLineMode = 0;

#endif // HAS_POPPLER_24

const int backend = 0;

} // Defaults

} // anonymous

namespace deepin_reader {

PDFAnnotation::PDFAnnotation(QMutex *mutex, Poppler::Annotation *annotation) : Annotation(),
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
    LOCK_ANNOTATION

    QList<QRectF> rectFList;

    if (m_annotation->subType() == Poppler::Annotation::AText) {
        rectFList.append(m_annotation->boundary().normalized());
    } else if (m_annotation->subType() == Poppler::Annotation::AHighlight) {
        QList<Poppler::HighlightAnnotation::Quad> quads = static_cast<Poppler::HighlightAnnotation *>(m_annotation)->highlightQuads();
        foreach (Poppler::HighlightAnnotation::Quad quad, quads) {
            QRectF rectbound;
            rectbound.setTopLeft(quad.points[0]);
            rectbound.setTopRight(quad.points[1]);
            rectbound.setBottomLeft(quad.points[3]);
            rectbound.setBottomRight(quad.points[2]);
            rectFList.append(rectbound);
        }
    }

    return rectFList;
}

QString PDFAnnotation::contents() const
{
    LOCK_ANNOTATION

    if (nullptr == m_annotation)
        return QString();

    return m_annotation->contents();
}

int PDFAnnotation::type()
{
    if (nullptr == m_annotation)
        return -1;

    return m_annotation->subType();
}

Poppler::Annotation *PDFAnnotation::ownAnnotation()
{
    return m_annotation;
}

bool PDFAnnotation::updateAnnotation(const QString contains, const QColor color)
{
    LOCK_ANNOTATION

    if (nullptr == m_annotation)
        return false;

    if (m_annotation->subType() == Poppler::Annotation::AText) {
        if (!contains.isEmpty())
            m_annotation->setContents(contains);
    } else if (m_annotation->subType() == Poppler::Annotation::AHighlight) {
        m_annotation->setContents(contains);
    }

    if (color.isValid()) {
        Poppler::Annotation::Style style;
        style.setColor(color);
        m_annotation->setStyle(style);
    }

    return true;
}

bool PDFAnnotation::setUniqueName(QString uniqueName) const
{
    LOCK_ANNOTATION

    if (nullptr == m_annotation)
        return false;

    m_annotation->setUniqueName(uniqueName);

    return true;
}

QString PDFAnnotation::uniqueName() const
{
    if (nullptr == m_annotation)
        return QString();

    return m_annotation->uniqueName();
}

PDFPage::PDFPage(QMutex *mutex, Poppler::Page *page) :
    m_mutex(mutex),
    m_page(page)
{
}

PDFPage::~PDFPage()
{
    textCache()->remove(this);

    LOCK_LONGTIMEOPERATION

    delete m_page;

    m_page = nullptr;
}

QSize PDFPage::size() const
{
    LOCK_PAGE

    return m_page->pageSize();
}

QSizeF PDFPage::sizeF() const
{
    LOCK_PAGE

    return m_page->pageSizeF();
}

QImage PDFPage::render(Dr::Rotation rotation, const double scaleFactor, const QRect &boundingRect) const
{
    return render(72 * scaleFactor, 72 * scaleFactor, rotation, boundingRect);
}

QImage PDFPage::render(int width, int height, Qt::AspectRatioMode) const
{
    qreal horizontalResolution = 72.0 * width / m_page->pageSize().width();
    qreal verticalResolution = 72.0 * height / m_page->pageSize().height();

    return render(horizontalResolution, verticalResolution, Dr::RotateBy0, QRect());
}

QImage PDFPage::render(qreal horizontalResolution, qreal verticalResolution, Dr::Rotation rotation, QRect boundingRect) const
{
    LOCK_LONGTIMEOPERATION

    if (m_page == nullptr)
        return QImage();

    Poppler::Page::Rotation rotate;

    switch (rotation) {
    default:
    case Dr::RotateBy0:
        rotate = Poppler::Page::Rotate0;
        break;
    case Dr::RotateBy90:
        rotate = Poppler::Page::Rotate90;
        break;
    case Dr::RotateBy180:
        rotate = Poppler::Page::Rotate180;
        break;
    case Dr::RotateBy270:
        rotate = Poppler::Page::Rotate270;
        break;
    }

    int x = -1;
    int y = -1;
    int w = -1;
    int h = -1;

    if (!boundingRect.isNull()) {
        x = boundingRect.x();
        y = boundingRect.y();
        w = boundingRect.width();
        h = boundingRect.height();
    }

    return m_page->renderToImage(horizontalResolution, verticalResolution, x, y, w, h, rotate);
}

QImage PDFPage::thumbnail() const
{
    LOCK_PAGE

    return m_page->thumbnail();
}

QString PDFPage::label() const
{
    LOCK_PAGE

    return m_page->label();
}

QList< Link * > PDFPage::links() const
{
    LOCK_PAGE

    QList< Link * > links;

    foreach (const Poppler::Link *link, m_page->links()) {
        const QRectF boundary = link->linkArea().normalized();

        if (link->linkType() == Poppler::Link::Goto) {
            const Poppler::LinkGoto *linkGoto = static_cast< const Poppler::LinkGoto * >(link);

            int page = linkGoto->destination().pageNumber();
            qreal left = qQNaN();
            qreal top = qQNaN();

            page = page >= 1 ? page : 1;

            if (linkGoto->destination().isChangeLeft()) {
                left = linkGoto->destination().left();

                left = left >= 0.0 ? left : 0.0;
                left = left <= 1.0 ? left : 1.0;
            }

            if (linkGoto->destination().isChangeTop()) {
                top = linkGoto->destination().top();

                top = top >= 0.0 ? top : 0.0;
                top = top <= 1.0 ? top : 1.0;
            }

            if (linkGoto->isExternal()) {
                links.append(new Link(boundary, linkGoto->fileName(), page));
            } else {
                links.append(new Link(boundary, page, left, top));
            }
        } else if (link->linkType() == Poppler::Link::Browse) {
            const Poppler::LinkBrowse *linkBrowse = static_cast< const Poppler::LinkBrowse * >(link);
            const QString url = linkBrowse->url();

            links.append(new Link(boundary, url));
        } else if (link->linkType() == Poppler::Link::Execute) {
            const Poppler::LinkExecute *linkExecute = static_cast< const Poppler::LinkExecute * >(link);
            const QString url = linkExecute->fileName();

            links.append(new Link(boundary, url));
        }

        delete link;
    }

    return links;
}

QString PDFPage::text(const QRectF &rect) const
{
    LOCK_PAGE

    return m_page->text(rect).simplified();
}

QString PDFPage::cachedText(const QRectF &rect) const
{
    TextBoxList textBoxes;

    if (!textCache()->object(this, textBoxes)) {
        {
            LOCK_PAGE

            foreach (Poppler::TextBox *textBox, m_page->textList()) {
                textBoxes.append(TextBox(textBox));
            }
        }

        textCache()->insert(this, textBoxes);
    }

    QString text;

    foreach (const TextBox &textBox, textBoxes) {
        if (!rect.intersects(textBox->boundingBox())) {
            continue;
        }

        const QString &characters = textBox->text();

        for (int index = 0; index < characters.length(); ++index) {
            if (rect.intersects(textBox->charBoundingBox(index))) {
                text.append(characters.at(index));
            }
        }

        if (textBox->hasSpaceAfter()) {
            text.append(QLatin1Char(' '));
        }
    }

    return text.simplified();
}

QList<Word> PDFPage::words(Dr::Rotation rotation)
{
    if (rotation == m_wordRotation)
        return m_words;

    m_wordRotation = rotation;

    Poppler::Page::Rotation rotate;

    switch (rotation) {
    default:
    case Dr::RotateBy0:
        rotate = Poppler::Page::Rotate0;
        break;
    case Dr::RotateBy90:
        rotate = Poppler::Page::Rotate90;
        break;
    case Dr::RotateBy180:
        rotate = Poppler::Page::Rotate180;
        break;
    case Dr::RotateBy270:
        rotate = Poppler::Page::Rotate270;
        break;
    }

    QList<Poppler::TextBox *> textBoxList = m_page->textList(rotate);
    for (int i = 0; i < textBoxList.count(); ++i) {
        Poppler::TextBox *box = textBoxList[i];
        if (nullptr == box)
            continue;

        for (int i = 0; i < box->text().count(); ++i) {
            Word word;
            word.text = box->text().at(i);
            word.boundingBox = box->charBoundingBox(i);
            m_words.append(word);

            if (box->hasSpaceAfter() && (i == (box->text().count() - 1))) {
                Poppler::TextBox *nextBox = box->nextWord();
                if (nextBox != nullptr) {
                    if (qFuzzyCompare(word.boundingBox.y(), nextBox->charBoundingBox(0).y())) {
                        Word spaceWord;
                        spaceWord.text = " ";
                        spaceWord.boundingBox = QRectF(word.boundingBox.x() + word.boundingBox.width(), word.boundingBox.y(), nextBox->charBoundingBox(0).x() - word.boundingBox.x() - word.boundingBox.width(), word.boundingBox.height());
                        m_words.append(spaceWord);
                    }
                }
            }
        }
    }

    return m_words;
}

QList< QRectF > PDFPage::search(const QString &text, bool matchCase, bool wholeWords) const
{
    LOCK_PAGE

    QList< QRectF > results;

#ifdef HAS_POPPLER_31

    const Poppler::Page::SearchFlags flags((matchCase ? 0 : Poppler::Page::IgnoreCase) | (wholeWords ? Poppler::Page::WholeWords : 0));

    results = m_page->search(text, flags);

#else

    Q_UNUSED(wholeWords);

    const Poppler::Page::SearchMode mode = matchCase ? Poppler::Page::CaseSensitive : Poppler::Page::CaseInsensitive;

#if defined(HAS_POPPLER_22)

    results = m_page->search(text, mode);

#elif defined(HAS_POPPLER_14)

    double left = 0.0, top = 0.0, right = 0.0, bottom = 0.0;

    while (m_page->search(text, left, top, right, bottom, Poppler::Page::NextResult, mode)) {
        results.append(QRectF(left, top, right - left, bottom - top));
    }

#else

    QRectF rect;

    while (m_page->search(text, rect, Poppler::Page::NextResult, mode)) {
        results.append(rect);
    }

#endif // HAS_POPPLER_22 HAS_POPPLER_14

#endif // HAS_POPPLER_31

    return results;
}

QList< Annotation * > PDFPage::annotations() const
{
    LOCK_PAGE

    QList< Annotation * > annotations;

    foreach (Poppler::Annotation *annotation, m_page->annotations()) {
        if (annotation->subType() == Poppler::Annotation::AText || annotation->subType() == Poppler::Annotation::AHighlight || annotation->subType() == Poppler::Annotation::AFileAttachment) {
            annotations.append(new PDFAnnotation(m_mutex, annotation));
            continue;
        }

        delete annotation;
    }

    return annotations;
}

bool PDFPage::canAddAndRemoveAnnotations() const
{
#ifdef HAS_POPPLER_20

    return true;

#else
    return false;

#endif // HAS_POPPLER_20
}

Annotation *PDFPage::addHighlightAnnotation(const QList<QRectF> &boundarys, const QString &text, const QColor &color)
{
    LOCK_PAGE

#ifdef HAS_POPPLER_20

    double smallestrx = 1.0, largestx = 0.0;
    double smallestry = 1.0, largestry = 0.0;
    QRectF tboundary;
    Poppler::Annotation::Style style;
    style.setColor(color);

    Poppler::Annotation::Popup popup;
    popup.setFlags(Poppler::Annotation::Hidden | Poppler::Annotation::ToggleHidingOnMouse);

    Poppler::HighlightAnnotation *annotation = new Poppler::HighlightAnnotation();

    QList< Poppler::HighlightAnnotation::Quad > quadList;

    foreach (QRectF boundary, boundarys) {
        Poppler::HighlightAnnotation::Quad quad;
        quad.points[0] = boundary.topLeft();
        quad.points[1] = boundary.topRight();
        quad.points[2] = boundary.bottomRight();
        quad.points[3] = boundary.bottomLeft();
        quadList.append(quad);
        smallestrx = smallestrx > boundary.x() ? boundary.x() : smallestrx;
        smallestry = smallestry > boundary.y() ? boundary.y() : smallestry;
        largestx = largestx < boundary.x() ? boundary.x() : largestx;
        largestry = largestry < boundary.y() ? boundary.y() : largestry;
    }

    //如果需要在其它文档中查看注释必须调用setBoundary(recboundary)设置点击范围
    tboundary.setX(smallestrx);
    tboundary.setY(smallestry);
    tboundary.setWidth(largestx - smallestrx);
    tboundary.setHeight(largestry - smallestrx);
    annotation->setBoundary(tboundary);

    annotation->setHighlightQuads(quadList);
    annotation->setStyle(style);
    annotation->setContents(text);
    annotation->setPopup(popup);

    m_page->addAnnotation(annotation);

    return new PDFAnnotation(m_mutex, annotation);

#else

    Q_UNUSED(boundary);
    Q_UNUSED(color);

    return 0;

#endif // HAS_POPPLER_20
}

bool PDFPage::removeAnnotation(deepin_reader::Annotation *annotation)
{
    LOCK_PAGE

#ifdef HAS_POPPLER_20

    deepin_reader::PDFAnnotation *PDFAnnotation = static_cast< deepin_reader::PDFAnnotation * >(annotation);

    if (PDFAnnotation == nullptr)
        return false;

    m_page->removeAnnotation(PDFAnnotation->m_annotation);

    PDFAnnotation->m_annotation = nullptr;
    delete PDFAnnotation;
    PDFAnnotation = nullptr;

#else

    Q_UNUSED(annotation);

#endif // HAS_POPPLER_20

    return true;
}

QList< FormField * > PDFPage::formFields() const
{
    LOCK_PAGE

    return QList<FormField * >();

//    QList< FormField * > formFields;

//    foreach (Poppler::FormField *formField, m_page->formFields()) {
//        if (!formField->isVisible() || formField->isReadOnly()) {
//            delete formField;
//            continue;
//        }

//        if (formField->type() == Poppler::FormField::FormText) {
//            Poppler::FormFieldText *formFieldText = static_cast< Poppler::FormFieldText * >(formField);

//            if (formFieldText->textType() == Poppler::FormFieldText::Normal || formFieldText->textType() == Poppler::FormFieldText::Multiline) {
//                formFields.append(new PDFFormField(m_mutex, formField));
//                continue;
//            }
//        } else if (formField->type() == Poppler::FormField::FormChoice) {
//            Poppler::FormFieldChoice *formFieldChoice = static_cast< Poppler::FormFieldChoice * >(formField);

//            if (formFieldChoice->choiceType() == Poppler::FormFieldChoice::ListBox || formFieldChoice->choiceType() == Poppler::FormFieldChoice::ComboBox) {
//                formFields.append(new PDFFormField(m_mutex, formField));
//                continue;
//            }
//        } else if (formField->type() == Poppler::FormField::FormButton) {
//            Poppler::FormFieldButton *formFieldButton = static_cast< Poppler::FormFieldButton * >(formField);

//            if (formFieldButton->buttonType() == Poppler::FormFieldButton::CheckBox || formFieldButton->buttonType() == Poppler::FormFieldButton::Radio) {
//                formFields.append(new PDFFormField(m_mutex, formField));
//                continue;
//            }
//        }

//        delete formField;
//    }

    //    return formFields;
}

bool PDFPage::updateAnnotation(Annotation *annotation, const QString &text, const QColor &color)
{
    if (nullptr == annotation)
        return false;

    if (m_page->annotations().contains(annotation->ownAnnotation())) {

        //set contains
        annotation->ownAnnotation()->setContents(text);

        //set Style(color)
        Poppler::Annotation::Style style =  annotation->ownAnnotation()->style();
        style.setColor(color);
        annotation->ownAnnotation()->setStyle(style);

        return true;
    }

    return false;
}

bool PDFPage::mouseClickIconAnnot(QPointF &clickPoint)
{
    foreach (Poppler::Annotation *annot, m_page->annotations()) {
        if (annot && annot->boundary().contains(clickPoint)) {
            qInfo() << "   22222222222   Annotation contents: " << annot->contents();
            return true;
        }
    }

    return false;
}

Annotation *PDFPage::addIconAnnotation(const QRectF rect, const QString text)
{
    if (nullptr == m_page)
        return nullptr;

    LOCK_PAGE

    QString strtype = "Note";
    Poppler::Annotation::Style style;
    style.setColor(Qt::yellow);

    Poppler::Annotation::Popup popup;
    popup.setFlags(Poppler::Annotation::Hidden | Poppler::Annotation::ToggleHidingOnMouse);

    Poppler::TextAnnotation *annotation = new Poppler::TextAnnotation(Poppler::TextAnnotation::Linked);

    annotation->setContents(text);
    annotation->setBoundary(rect);
    annotation->setTextIcon(strtype);
    annotation->setStyle(style);
    annotation->setPopup(popup);
    annotation->setFlags(annotation->flags() | Poppler::Annotation::FixedRotation);
    m_page->addAnnotation(annotation);

    return (new PDFAnnotation(m_mutex, annotation));
}

Annotation *PDFPage::moveIconAnnotation(Annotation *annot, const QRectF rect)
{
    if (nullptr == m_page && nullptr == annot)
        return nullptr;

    LOCK_PAGE

    if (annot->ownAnnotation()) {
        //set rect
        annot->ownAnnotation()->setBoundary(rect);

        return annot;
    }
    return nullptr;
}

PDFDocument::PDFDocument(Poppler::Document *document) :
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
    LOCK_DOCUMENT

    return m_document->numPages();
}

Page *PDFDocument::page(int index) const
{
    LOCK_DOCUMENT

    if (Poppler::Page *page = m_document->page(index)) {
        return new PDFPage(&m_mutex, page);
    }

    return nullptr;
}

bool PDFDocument::isLocked() const
{
    LOCK_DOCUMENT

    return m_document->isLocked();
}

bool PDFDocument::unlock(const QString &password)
{
    LOCK_DOCUMENT

    // Poppler drops render hints and backend after unlocking so we need to restore them.

    const Poppler::Document::RenderHints hints = m_document->renderHints();
    const Poppler::Document::RenderBackend backend = m_document->renderBackend();

    const bool ok = m_document->unlock(password.toLatin1(), password.toLatin1());

    restoreRenderHint(m_document, hints, Poppler::Document::Antialiasing);
    restoreRenderHint(m_document, hints, Poppler::Document::TextAntialiasing);

#ifdef HAS_POPPLER_14

    restoreRenderHint(m_document, hints, Poppler::Document::TextHinting);

#endif // HAS_POPPLER_14

#ifdef HAS_POPPLER_18

    restoreRenderHint(m_document, hints, Poppler::Document::TextSlightHinting);

#endif // HAS_POPPLER_18

#ifdef HAS_POPPLER_35

    restoreRenderHint(m_document, hints, Poppler::Document::IgnorePaperColor);

#endif // HAS_POPPLER_35

#ifdef HAS_POPPLER_22

    restoreRenderHint(m_document, hints, Poppler::Document::OverprintPreview);

#endif // HAS_POPPLER_22

#ifdef HAS_POPPLER_24

    restoreRenderHint(m_document, hints, Poppler::Document::ThinLineSolid);
    restoreRenderHint(m_document, hints, Poppler::Document::ThinLineShape);

#endif // HAS_POPPLER_24

    m_document->setRenderBackend(backend);

    return ok;
}

QStringList PDFDocument::saveFilter() const
{
    return QStringList() << "Portable document format (*.pdf)";
}

bool PDFDocument::canSave() const
{
    return true;
}

bool PDFDocument::save(const QString &filePath, bool withChanges) const
{
    LOCK_DOCUMENT

    QScopedPointer< Poppler::PDFConverter > pdfConverter(m_document->pdfConverter());

    pdfConverter->setOutputFileName(filePath);

    Poppler::PDFConverter::PDFOptions options = pdfConverter->pdfOptions();

    if (withChanges) {
        options |= Poppler::PDFConverter::WithChanges;
    }

    pdfConverter->setPDFOptions(options);

    return pdfConverter->convert();
}

bool PDFDocument::canBePrintedUsingCUPS() const
{
    return true;
}

void PDFDocument::setPaperColor(const QColor &paperColor)
{
    LOCK_DOCUMENT

    m_document->setPaperColor(paperColor);
}

Outline PDFDocument::outline() const
{
    Outline outline;

    LOCK_DOCUMENT

    QScopedPointer< QDomDocument > toc(m_document->toc());

    if (toc) {
        outline = loadOutline(*toc, m_document);
    }

    return outline;
}

Properties PDFDocument::properties() const
{
    Properties properties;

    LOCK_DOCUMENT

    foreach (const QString &key, m_document->infoKeys()) {
        QString value = m_document->info(key);

        if (value.startsWith("D:")) {
            value = m_document->date(key).toString();
        }

        properties.insert(key, value);
    }

    int pdfMajorVersion = 1;
    int pdfMinorVersion = 0;

    m_document->getPdfVersion(&pdfMajorVersion, &pdfMinorVersion);

    properties.insert("Version", QString("v.%1.%2").arg(pdfMajorVersion).arg(pdfMinorVersion));
    properties.insert("Encrypted", m_document->isEncrypted());
    properties.insert("Linearized", m_document->isLinearized());
    properties.insert("KeyWords", m_document->keywords());
    properties.insert("Title", m_document->title());
    properties.insert("Creator", m_document->creator());
    properties.insert("Producer", m_document->producer());

    return properties;
}

QAbstractItemModel *PDFDocument::fonts() const
{
    LOCK_DOCUMENT

    return new FontsModel(m_document->fonts());
}

bool PDFDocument::wantsContinuousMode() const
{
    LOCK_DOCUMENT

    const Poppler::Document::PageLayout pageLayout = m_document->pageLayout();

    return pageLayout == Poppler::Document::OneColumn
           || pageLayout == Poppler::Document::TwoColumnLeft
           || pageLayout == Poppler::Document::TwoColumnRight;
}

bool PDFDocument::wantsSinglePageMode() const
{
    LOCK_DOCUMENT

    const Poppler::Document::PageLayout pageLayout = m_document->pageLayout();

    return pageLayout == Poppler::Document::SinglePage
           || pageLayout == Poppler::Document::OneColumn;
}

bool PDFDocument::wantsTwoPagesMode() const
{
    LOCK_DOCUMENT

    const Poppler::Document::PageLayout pageLayout = m_document->pageLayout();

    return pageLayout == Poppler::Document::TwoPageLeft
           || pageLayout == Poppler::Document::TwoColumnLeft;
}

bool PDFDocument::wantsTwoPagesWithCoverPageMode() const
{
    LOCK_DOCUMENT

    const Poppler::Document::PageLayout pageLayout = m_document->pageLayout();

    return pageLayout == Poppler::Document::TwoPageRight
           || pageLayout == Poppler::Document::TwoColumnRight;
}

bool PDFDocument::wantsRightToLeftMode() const
{
#ifdef HAS_POPPLER_26

    return m_document->textDirection() == Qt::RightToLeft;

#else

    return false;

#endif // HAS_POPPLER_26
}

PDFDocument *PDFDocument::loadDocument(const QString &filePath, const QString &password)
{
    if (Poppler::Document *document = Poppler::Document::load(filePath, QByteArray(), QByteArray().append(password))) {
        document->setRenderHint(Poppler::Document::Antialiasing, true);
        document->setRenderHint(Poppler::Document::TextAntialiasing, true);
//        document->setRenderHint(Poppler::Document::TextHinting, true);
//        document->setRenderHint(Poppler::Document::TextSlightHinting, true);
        document->setRenderHint(Poppler::Document::IgnorePaperColor, false);
//        document->setRenderHint(Poppler::Document::OverprintPreview, true);
//        document->setRenderHint(Poppler::Document::ThinLineSolid, true);
//        document->setRenderHint(Poppler::Document::ThinLineShape, true);
        document->setRenderHint(Poppler::Document::OverprintPreview, false);

        return new deepin_reader::PDFDocument(document);
    }

    return nullptr;
}

}


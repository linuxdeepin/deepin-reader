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
    ~PDFAnnotation();

    QList<QRectF> boundary() const;

    QString contents() const;

    int type();

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

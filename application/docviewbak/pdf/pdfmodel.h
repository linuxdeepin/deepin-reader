#ifndef PDFMODEL_H
#define PDFMODEL_H

#include <QCoreApplication>
#include <QMutex>
#include <QScopedPointer>

#define LOCK_ANNOTATION QMutexLocker mutexLocker(m_mutex);
#define LOCK_FORM_FIELD QMutexLocker mutexLocker(m_mutex);
#define LOCK_PAGE QMutexLocker mutexLocker(m_mutex);
#define LOCK_DOCUMENT QMutexLocker mutexLocker(&m_mutex);

class QCheckBox;
class QComboBox;
class QFormLayout;
class QSettings;

namespace Poppler
{
class Annotation;
class Document;
class FormField;
class Page;
}
#include "../model.h"

class PdfAnnotation : public ModelAnnotation
{
    Q_OBJECT

    friend class PdfPage;

public:
    ~PdfAnnotation();

    QRectF boundary() const;
    QString contents() const;

    QWidget* createWidget();

private:
    Q_DISABLE_COPY(PdfAnnotation)

    PdfAnnotation(QMutex* mutex, Poppler::Annotation* annotation);

    mutable QMutex* m_mutex;
    Poppler::Annotation* m_annotation;

};

class PdfFormField : public ModelFormField
{
    Q_OBJECT

    friend class PdfPage;

public:
    ~PdfFormField();

    QRectF boundary() const;
    QString name() const;

    QWidget* createWidget();

private:
    Q_DISABLE_COPY(PdfFormField)

    PdfFormField(QMutex* mutex, Poppler::FormField* formField);

    mutable QMutex* m_mutex;
    Poppler::FormField* m_formField;

};

class PdfPage : public ModelPage
{
    Q_DECLARE_TR_FUNCTIONS(PdfPage)

    friend class PdfDocument;

public:
    ~PdfPage();

   QSizeF size() const;

   QImage render(qreal horizontalResolution, qreal verticalResolution, Rotation rotation, QRect boundingRect) const;

//    QString label() const;

//    QList< Link* > links() const;

//    QString text(const QRectF& rect) const;
//    QString cachedText(const QRectF& rect) const;

//    QList< QRectF > search(const QString& text, bool matchCase, bool wholeWords) const;

//    QList< Annotation* > annotations() const;

//    bool canAddAndRemoveAnnotations() const;
//    Annotation* addTextAnnotation(const QRectF& boundary, const QColor& color);
//    Annotation* addHighlightAnnotation(const QRectF& boundary, const QColor& color);
//    void removeAnnotation(Annotation* annotation);

//    QList< ModelFormField* > formFields() const;

private:
    Q_DISABLE_COPY(PdfPage)

    PdfPage(QMutex* mutex, Poppler::Page* page);

    mutable QMutex* m_mutex;
    Poppler::Page* m_page;

};

class PdfDocument : public ModelDocument
{
    Q_DECLARE_TR_FUNCTIONS(Model::PdfDocument)

    friend class PdfPlugin;

public:
    ~PdfDocument();

    int numberOfPages() const;

    ModelPage* page(int index) const;

    bool isLocked() const;
//    bool unlock(const QString& password);

//    QStringList saveFilter() const;

//    bool canSave() const;
//    bool save(const QString& filePath, bool withChanges) const;

//    bool canBePrintedUsingCUPS() const;

//    void setPaperColor(const QColor& paperColor);

//    Outline outline() const;
//    Properties properties() const;

//    QAbstractItemModel* fonts() const;

//    bool wantsContinuousMode() const;
//    bool wantsSinglePageMode() const;
//    bool wantsTwoPagesMode() const;
//    bool wantsTwoPagesWithCoverPageMode() const;
//    bool wantsRightToLeftMode() const;

private:
    Q_DISABLE_COPY(PdfDocument)

    PdfDocument(Poppler::Document* document);

    mutable QMutex m_mutex;
    Poppler::Document* m_document;
};

class PdfPlugin :public QObject, Plugin
{
    Q_OBJECT

public:
    PdfPlugin(QObject* parent = 0);

    ModelDocument* loadDocument(const QString& filePath) const;

    //SettingsWidget* createSettingsWidget(QWidget* parent) const;

private:
    Q_DISABLE_COPY(PdfPlugin)

    QSettings* m_settings;

};

#endif // PDFMODEL_H

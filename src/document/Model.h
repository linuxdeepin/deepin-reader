#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include <QList>
#include <QtPlugin>
#include <QRect>
#include <QStandardItemModel>
#include <QString>
#include <QWidget>

class QColor;
class QImage;
class QSizeF;

#include "Global.h"

namespace deepin_reader {
struct Link {
    QPainterPath boundary;
    int page;
    qreal left;
    qreal top;
    QString urlOrFileName;
    Link() : boundary(), page(-1), left(0.0), top(0.0), urlOrFileName() {}
    Link(const QPainterPath &boundary, int page, qreal left = 0.0, qreal top = 0.0) : boundary(boundary), page(page), left(left), top(top), urlOrFileName() {}
    Link(const QRectF &boundingRect, int page, qreal left = 0.0, qreal top = 0.0) : boundary(), page(page), left(left), top(top), urlOrFileName() { boundary.addRect(boundingRect); }
    Link(const QPainterPath &boundary, const QString &url) : boundary(boundary), page(-1), left(0.0), top(0.0), urlOrFileName(url) {}
    Link(const QRectF &boundingRect, const QString &url) : boundary(), page(-1), left(0.0), top(0.0), urlOrFileName(url) { boundary.addRect(boundingRect); }
    Link(const QPainterPath &boundary, const QString &fileName, int page) : boundary(boundary), page(page), left(0.0), top(0.0), urlOrFileName(fileName) {}
    Link(const QRectF &boundingRect, const QString &fileName, int page) : boundary(), page(page), left(0.0), top(0.0), urlOrFileName(fileName) { boundary.addRect(boundingRect); }
};

struct Section;

typedef QVector< Section > Outline;

typedef QVector< QPair< QString, QString > > Properties;

struct Section {
    QString title;
    Link link;
    Outline children;
};

struct Word {
    QString text;
    QRectF boundingBox;
};

struct SearchResult {
    unsigned int page = 0;
    QList<Word> words;
};

class Annotation : public QObject
{
    Q_OBJECT
public:
    Annotation() : QObject() {}
    virtual ~Annotation() {}
    virtual QList<QRectF> boundary() const = 0;
    virtual QString contents() const = 0;
    virtual int type() = 0;

    //virtual QWidget *createWidget() = 0;
signals:
    void wasModified();
};

class FormField : public QObject
{
    Q_OBJECT
public:
    FormField() : QObject() {}
    virtual ~FormField() {}
    virtual QRectF boundary() const = 0;
    virtual QString name() const = 0;
    virtual QWidget *createWidget() = 0;
signals:
    void wasModified();
};

class Page: public QObject
{
    Q_OBJECT
public:
    Page() : QObject() {}
    virtual ~Page() {}
//    virtual QSizeF size() const = 0;
//    virtual QImage render(qreal horizontalResolution = 72.0, qreal verticalResolution = 72.0, Dr::Rotation rotation = Dr::RotateBy0, const QRect &boundingRect = QRect()) const = 0;
//    virtual QImage render(qreal horizontalResolution = 72.0, qreal verticalResolution = 72.0, Dr::Rotation rotation = Dr::RotateBy0, const double scale = 100.0) const = 0;
    virtual QSize size() const = 0;
    virtual QSizeF sizeF() const = 0;
    virtual QImage render(Dr::Rotation rotation = Dr::RotateBy0, const double scaleFactor = 1.00, const QRect &boundingRect = QRect()) const = 0;
    virtual QImage render(int width, int height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio) const = 0;
    virtual QString label() const { return QString(); }
    virtual QList< Link * > links() const { return QList< Link * >(); }
    virtual QString text(const QRectF &rect) const { Q_UNUSED(rect) return QString(); }
    virtual QString cachedText(const QRectF &rect) const { return text(rect); }
    virtual QList< QRectF > search(const QString &text, bool matchCase, bool wholeWords) const { Q_UNUSED(text) Q_UNUSED(matchCase) Q_UNUSED(wholeWords) return QList< QRectF >(); }
    virtual QList< Annotation * > annotations() const { return QList< Annotation * >(); }
    virtual bool canAddAndRemoveAnnotations() const { return false; }
    virtual Annotation *addTextAnnotation(const QRectF &boundary, const QColor &color) { Q_UNUSED(boundary) Q_UNUSED(color) return 0; }
    virtual Annotation *addHighlightAnnotation(const QRectF &boundary, const QColor &color) { Q_UNUSED(boundary) Q_UNUSED(color) return 0; }
    virtual void removeAnnotation(Annotation *annotation) { Q_UNUSED(annotation)}
    virtual QList< FormField * > formFields() const { return QList< FormField * >(); }
    virtual QList<Word> words(Dr::Rotation rotation)const {Q_UNUSED(rotation) return QList<Word>();}
};

class Document: public QObject
{
    Q_OBJECT
public:
    Document() : QObject() {}
    virtual ~Document() {}
    virtual int numberOfPages() const = 0;
    virtual Page *page(int index) const = 0;
    virtual bool isLocked() const { return false; }
    virtual bool unlock(const QString &password) { Q_UNUSED(password); return false; }
    virtual QStringList saveFilter() const { return QStringList(); }
    virtual bool canSave() const { return false; }
    virtual bool save(const QString &filePath, bool withChanges) const { Q_UNUSED(filePath); Q_UNUSED(withChanges); return false; }
    virtual bool canBePrintedUsingCUPS() const { return false; }
    virtual void setPaperColor(const QColor &paperColor) { Q_UNUSED(paperColor); }
    virtual Outline outline() const { return Outline(); }
    virtual Properties properties() const { return Properties(); }
    virtual QAbstractItemModel *fonts() const { return 0; }
    virtual bool wantsContinuousMode() const { return false; }
    virtual bool wantsSinglePageMode() const { return false; }
    virtual bool wantsTwoPagesMode() const { return false; }
    virtual bool wantsTwoPagesWithCoverPageMode() const { return false; }
    virtual bool wantsRightToLeftMode() const { return false; }
};
} // deepin_reader


#endif // DOCUMENTMODEL_H

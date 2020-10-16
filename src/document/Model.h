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
#ifndef DOCUMENTMODEL_H
#define DOCUMENTMODEL_H

#include "Global.h"

#include <QList>
#include <QtPlugin>
#include <QRect>
#include <QStandardItemModel>
#include <QString>
#include <QWidget>
#include <QDateTime>
#include <QPainterPath>
#include <QPointF>
#include <QRectF>

class QColor;
class QImage;
class QSizeF;
class DPdfAnnot;

namespace deepin_reader {

const int Z_ORDER_HIGHLIGHT   = 2;
const int Z_ORDER_SELECT_TEXT = 3;
const int Z_ORDER_ICON        = 4;

struct Link {
    QPainterPath boundary;
    int page = -1;
    qreal left = 0;
    qreal top = 0;
    QString urlOrFileName;
    Link() : boundary(), page(-1), left(0.0), top(0.0), urlOrFileName() {}
    Link(const QPainterPath &boundary, int page, qreal left = 0.0, qreal top = 0.0) : boundary(boundary), page(page), left(left), top(top), urlOrFileName() {}
    Link(const QRectF &boundingRect, int page, qreal left = 0.0, qreal top = 0.0) : boundary(), page(page), left(left), top(top), urlOrFileName() { boundary.addRect(boundingRect); }
    Link(const QPainterPath &boundary, const QString &url) : boundary(boundary), page(-1), left(0.0), top(0.0), urlOrFileName(url) {}
    Link(const QRectF &boundingRect, const QString &url) : boundary(), page(-1), left(0.0), top(0.0), urlOrFileName(url) { boundary.addRect(boundingRect); }
    Link(const QPainterPath &boundary, const QString &fileName, int page) : boundary(boundary), page(page), left(0.0), top(0.0), urlOrFileName(fileName) {}
    Link(const QRectF &boundingRect, const QString &fileName, int page) : boundary(), page(page), left(0.0), top(0.0), urlOrFileName(fileName) { boundary.addRect(boundingRect); }

    bool isValid()
    {
        return page >= 1 || !urlOrFileName.isEmpty();
    }
};

struct Section;

typedef QVector< Section > Outline;

typedef QMap<QString, QVariant> Properties;

struct Section {
    int nIndex;
    QPointF offsetPointF;
    QString title;
    Outline children;
};

struct Word {
    QString text;

    QRectF boundingBox;

    QRectF wordBoundingRect() const
    {
        return boundingBox;
    }

    Word()
    {

    }

    Word(const QString &text, const QRectF &rectf)
    {
        this->text = text;
        this->boundingBox = rectf;
    }
};

struct SearchResult {
    int page = 0;
    QList<Word> words;
};

struct FileInfo {
    QString filePath;
    QString theme;
    QString auther;
    QString keyword;
    QString producter;
    QString creater;
    QDateTime createTime;
    QDateTime changeTime;
    QString format;
    unsigned int width = 0;
    unsigned int height = 0;
    unsigned int numpages = 0;
    bool     optimization = false;
    bool     safe  = false;
    float    size = 0;
};

class Annotation : public QObject
{
    Q_OBJECT
public:
    Annotation() : QObject() {}

    virtual ~Annotation() {}

    virtual QList<QRectF> boundary() const = 0;

    virtual QString contents() const = 0;

    virtual DPdfAnnot *ownAnnotation() = 0;

    virtual QString uniqueName() const {return QString();}

    virtual bool setUniqueName(QString uniqueName) const {Q_UNUSED(uniqueName) return false;}

    //数值同poppler类型
    enum AnnotationType {
        AUnknown = 0,         ///< 前期支持以外的
        AText = 1,            ///< TextAnnotation
        AHighlight = 2,       ///< HighlightAnnotation
        ALink = 3
    };

    virtual int type() = 0;

    int page = 0;

signals:
    void wasModified();
};
typedef QList<Annotation *>  AnnotationList;

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

    virtual QSizeF sizeF() const = 0;
    virtual QImage render(const qreal & scaleFactor) const { Q_UNUSED(scaleFactor) return QImage();}
    virtual QImage render(Dr::Rotation rotation, const double scaleFactor, const QRectF &boundingRect = QRectF()) const = 0;
    virtual QImage render(int width, int height, Qt::AspectRatioMode mode = Qt::IgnoreAspectRatio) const = 0;
    virtual Link getLinkAtPoint(const QPointF &) const { return Link(); }
    virtual QString text(const QRectF &rect) const { Q_UNUSED(rect) return QString(); }
    virtual QString cachedText(const QRectF &rect) const { return text(rect); }
    virtual QVector<QRectF> search(const QString &text, bool matchCase, bool wholeWords) const { Q_UNUSED(text) Q_UNUSED(matchCase) Q_UNUSED(wholeWords) return QVector<QRectF>(); }
    virtual QList< Annotation * > annotations() const { return QList< Annotation * >(); }
    virtual bool canAddAndRemoveAnnotations() const { return false; }
    virtual Annotation *addHighlightAnnotation(const QList<QRectF> &boundarys, const QString &text, const QColor &color) { Q_UNUSED(boundarys) Q_UNUSED(text) Q_UNUSED(color) return nullptr; }
    virtual bool removeAnnotation(Annotation *annotation) { Q_UNUSED(annotation) return  false;}
    virtual QList< FormField * > formFields() const { return QList< FormField * >(); }
    virtual QList<Word> words() {return QList<Word>();}
    virtual bool mouseClickIconAnnot(QPointF &) {return false;}
    virtual bool updateAnnotation(Annotation *, const QString &, const QColor &) {return false;};
    virtual Annotation *addIconAnnotation(const QRectF &ponit, const QString &text) { Q_UNUSED(ponit) Q_UNUSED(text) return nullptr; }
    virtual Annotation *moveIconAnnotation(Annotation *annot, const QRectF &rect) { Q_UNUSED(annot) Q_UNUSED(rect) return nullptr; }
};

class Document: public QObject
{
    Q_OBJECT
public:
    enum DocStatus {
        NOT_LOADED = -1,
        SUCCESS = 0,
        FILE_ERROR,
        FORMAT_ERROR,
        PASSWORD_ERROR,
        HANDLER_ERROR,
        FILE_NOT_FOUND_ERROR
    };

    Document() : QObject() {}
    virtual ~Document() {}
    virtual int numberOfPages() const = 0;
    virtual Page *page(int index) const = 0;
    virtual QSizeF pageSizeF(int) const { return QSizeF(0, 0); };
    virtual QStringList saveFilter() const { return QStringList(); }
    virtual QString label(int) const { return QString(); };
    virtual bool save(const QString &filePath) const { Q_UNUSED(filePath); return false; }
    virtual bool saveAs(const QString &filePath) const { Q_UNUSED(filePath); return false; }
    virtual bool canBePrintedUsingCUPS() const { return false; }
    virtual void setPaperColor(const QColor &paperColor) { Q_UNUSED(paperColor); }
    virtual Outline outline() const { return Outline(); }
    virtual Properties properties() const { return Properties(); }
    virtual QAbstractItemModel *fonts() const { return nullptr; }
    virtual bool wantsContinuousMode() const { return false; }
    virtual bool wantsSinglePageMode() const { return false; }
    virtual bool wantsTwoPagesMode() const { return false; }
    virtual bool wantsTwoPagesWithCoverPageMode() const { return false; }
    virtual bool wantsRightToLeftMode() const { return false; }
};

} // deepin_reader


#endif // DOCUMENTMODEL_H

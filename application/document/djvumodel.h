/*

Copyright 2013 Adam Reichold
Copyright 2013 Alexander Volkov

This file is part of qpdfview.

The implementation is based on KDjVu by Pino Toscano.

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

#ifndef DJVUMODEL_H
#define DJVUMODEL_H

#include "model.h"

#include <QHash>
#include <QMutex>

typedef struct ddjvu_context_s ddjvu_context_t;
typedef struct ddjvu_format_s ddjvu_format_t;
typedef struct ddjvu_document_s ddjvu_document_t;
typedef struct ddjvu_pageinfo_s ddjvu_pageinfo_t;

namespace deepin_reader {

class DjVuPage : public Page
{
    friend class DjVuDocument;

public:
    ~DjVuPage();

    QSizeF size() const;

    QImage render(qreal horizontalResolution, qreal verticalResolution, Dr::Rotation rotation, const QRect &boundingRect) const;

    QImage render(qreal horizontalResolution = 72.0, qreal verticalResolution = 72.0, Dr::Rotation rotation = Dr::RotateBy0, const double scale = 100.00) const;

    QList< Link * > links() const;

    QString text(const QRectF &rect) const;

    QList< QRectF > search(const QString &text, bool matchCase) const;

private:
    Q_DISABLE_COPY(DjVuPage)

    DjVuPage(const class DjVuDocument *parent, int index, const ddjvu_pageinfo_t &pageinfo);

    const class DjVuDocument *m_parent;

    int m_index;

    QSizeF m_size;

    int m_resolution;

};

class DjVuDocument : public Document
{
    friend class DjVuPage;

public:
    ~DjVuDocument();

    int numberOfPages() const;

    Page *page(int index) const;

    QStringList saveFilter() const;

    bool canSave() const;
    bool save(const QString &filePath, bool withChanges) const;

    void loadOutline(QStandardItemModel *outlineModel) const;
    void loadProperties(QStandardItemModel *propertiesModel) const;

    static deepin_reader::DjVuDocument *loadDocument(const QString &filePath);

private:
    Q_DISABLE_COPY(DjVuDocument)

    DjVuDocument(ddjvu_context_t *context, ddjvu_document_t *document);

    mutable QMutex m_mutex;

    ddjvu_context_t *m_context;
    ddjvu_document_t *m_document;
    ddjvu_format_t *m_format;

    QHash< QString, int > m_indexByName;

    void prepareIndexByName();

};

} // deepin_reader

#endif // DJVUMODEL_H

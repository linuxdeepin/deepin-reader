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
#ifndef DJVUMODEL_H
#define DJVUMODEL_H

#include "Model.h"

#include <QHash>
#include <QMutex>
#include <QRectF>

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

    QSize size() const;

    QSizeF sizeF() const;

    QImage render(qreal width, qreal height, Qt::AspectRatioMode mode)const;     //按大小获取

    QImage render(Dr::Rotation rotation, const double scaleFactor, const QRectF &boundingRect = QRectF()) const;//按缩放比例获取; boundingRect:取其中某一区域图片

    QString label() const;

    QList< Link * > links() const;

    QString text(const QRectF &rect) const;

    QVector< QRectF > search(const QString &text, bool matchCase, bool wholeWords) const;

private:
    Q_DISABLE_COPY(DjVuPage)

    DjVuPage(const class DjVuDocument *parent, int index, const ddjvu_pageinfo_t &pageinfo);

    const class DjVuDocument *m_parent;

    int m_index;
    QSize m_size;
    int m_resolution;

};

class DjVuDocument : public Document
{
    friend class DjVuPage;
public:
    ~DjVuDocument();

    int numberOfPages() const override;

    Page *page(int index) const override;

    QStringList saveFilter() const override;

    bool save(const QString &filePath) const override;

    bool saveAs(const QString &filePath) const override;

    Outline outline() const override;

    Properties properties() const override;

    static deepin_reader::DjVuDocument *loadDocument(const QString &filePath);

private:
    Q_DISABLE_COPY(DjVuDocument)

    DjVuDocument(ddjvu_context_t *context, ddjvu_document_t *document);

    mutable QMutex m_mutex;
    mutable QMutex *m_globalMutex;

    ddjvu_context_t *m_context;
    ddjvu_document_t *m_document;
    ddjvu_format_t *m_format;

    QHash< QString, int > m_pageByName;
    QHash< int, QString > m_titleByIndex;

    void prepareFileInfo();

};
} // deepin_reader

#endif // DJVUMODEL_H

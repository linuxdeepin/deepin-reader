#ifndef DJVUMODEL_H
#define DJVUMODEL_H

#include "model.h"

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

    QImage render(int width, int height, Qt::AspectRatioMode mode)const;     //按大小获取

    QImage render(Dr::Rotation rotation, const double scaleFactor, const QRect &boundingRect = QRect()) const;//按缩放比例获取; boundingRect:取其中某一区域图片

    QList< Link * > links() const;

    QString text(const QRectF &rect) const;

    QList< QRectF > search(const QString &text, bool matchCase) const;

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

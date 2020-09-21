#ifndef DPdfiumPAGE_H
#define DPdfiumPAGE_H

#include <QObject>
#include <QImage>
#include <QSharedPointer>

#include "dpdfiumglobal.h"

class DPdfium;
class CPDF_Page;
class CPDF_TextPage;
class CPDF_Document;

class PageHolder {
public:
    QWeakPointer<CPDF_Document> m_doc;
    CPDF_Page *m_page;
    CPDF_TextPage *m_textPage;
    int i;
    PageHolder(QWeakPointer<CPDF_Document> doc, CPDF_Page *page);
    ~PageHolder();
};

class Q_PDFIUM_EXPORT DPdfiumPage
{
public:
    DPdfiumPage(const DPdfiumPage &other);
    DPdfiumPage &operator=(const DPdfiumPage &other);
    virtual ~DPdfiumPage();

    qreal width() const;
    qreal height() const;

    bool isValid() const;
    int pageIndex() const;
    QImage image(qreal scale = 1.0);

    int countChars() const;
    QVector<QRectF> getTextRects(int start = 0, int charCount = -1) const;
    QString text(const QRectF &rect) const;
    QString text() const;
    QString text(int start, int charCount) const;

private:
    DPdfiumPage(QSharedPointer<PageHolder> page, int pageIndex);

    QSharedPointer<PageHolder> m_pageHolder;
    int m_index;

    friend class DPdfium;
};

#endif // DPdfiumPAGE_H

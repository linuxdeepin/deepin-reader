#ifndef DPdfium_H
#define DPdfium_H

#include <QObject>
#include <QMap>
#include <QWeakPointer>
#include <QSharedPointer>
#include <QVector>

#include "dpdfiumpage.h"
#include "dpdfiumglobal.h"

QT_BEGIN_NAMESPACE

class CPDF_Document;
class CPDF_Page;
class PageHolder;

class Q_PDFIUM_EXPORT DPdfium
{

public:
    enum Status {
        NOT_LOADED = -1,
        SUCCESS = 0,
        FILE_ERROR,
        FORMAT_ERROR,
        PASSWORD_ERROR,
        HANDLER_ERROR,
        FILE_NOT_FOUND_ERROR
    };


    explicit DPdfium();
    DPdfium(QString filename, QString password = QString());

    virtual ~DPdfium();

    bool isValid() const;
    QString filename() const;
    int pageCount() const;
    Status status() const;
    DPdfiumPage page(int i);

public Q_SLOTS:
    Status loadFile(QString filename, QString password = QString());

private:
    Q_DISABLE_COPY(DPdfium)

    QSharedPointer<CPDF_Document> m_document;
    QVector<QWeakPointer<PageHolder>> m_pages;
    QString m_filename;
    int m_pageCount;
    Status m_status;
    DPdfium::Status parseError(int err);
};

QT_END_NAMESPACE

#endif // DPdfium_H

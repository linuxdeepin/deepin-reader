#include "dpdfium.h"
#include "../3rdparty/pdfium/public/fpdfview.h"
#include "../3rdparty/pdfium/core/fpdfapi/parser/cpdf_document.h"
#include "../3rdparty/pdfium/core/fpdfapi/page/cpdf_page.h"
#include <QFile>

QT_BEGIN_NAMESPACE

DPdfium::DPdfium()
    : m_document(nullptr)
    , m_pageCount(0)
    , m_status(NOT_LOADED)
{
}

DPdfium::DPdfium(QString filename, QString password)
    : m_document(nullptr)
    , m_pageCount(0)
    , m_status(NOT_LOADED)
{
    loadFile(filename, password);
}

DPdfium::~DPdfium() {
    m_pages.clear();
    m_document.clear();
}

bool DPdfium::isValid() const
{
    return m_document != NULL;
}

DPdfium::Status DPdfium::loadFile(QString filename, QString password)
{
    m_filename = filename;

    m_pages.clear();
    m_document.clear();

    // Pdfium API enable creating new pdfs, but
    // we are using only for read. So we will
    // return error if file does not exists
    if (!QFile::exists(filename)) {
        m_status = FILE_NOT_FOUND_ERROR;
        return m_status;
    }

    void* ptr = FPDF_LoadDocument(m_filename.toUtf8().constData(),
                                  password.toUtf8().constData());
    auto doc = static_cast<CPDF_Document*>(ptr);

    m_document.reset(doc);
    m_status = m_document ? SUCCESS : parseError(FPDF_GetLastError());

    if (m_document){
        m_pageCount = m_document->GetPageCount();
        m_pages.resize(m_pageCount);
    }

    return m_status;
}

DPdfium::Status DPdfium::parseError(int err) {
    DPdfium::Status err_code = DPdfium::SUCCESS;
    // Translate FPDFAPI error code to FPDFVIEW error code
    switch (err) {
    case FPDF_ERR_SUCCESS:
        err_code = DPdfium::SUCCESS;
        break;
    case FPDF_ERR_FILE:
        err_code = DPdfium::FILE_ERROR;
        break;
    case FPDF_ERR_FORMAT:
        err_code = DPdfium::FORMAT_ERROR;
        break;
    case FPDF_ERR_PASSWORD:
        err_code = DPdfium::PASSWORD_ERROR;
        break;
    case FPDF_ERR_SECURITY:
        err_code = DPdfium::HANDLER_ERROR;
        break;
    }
    return err_code;
}

QString DPdfium::filename() const
{
    return m_filename;
}

int DPdfium::pageCount() const
{
    return m_pageCount;
}

DPdfium::Status DPdfium::status() const
{
    return m_status;
}

DPdfiumPage DPdfium::page(int i)
{
    Q_ASSERT( i < m_pageCount && i >=0 );

    auto strongRef = m_pages[i].toStrongRef();
    if (!strongRef)
        strongRef.reset(new PageHolder(m_document.toWeakRef(),
                                       static_cast<CPDF_Page*>
                                       (FPDF_LoadPage(m_document.data(), i))));

    m_pages[i] = strongRef.toWeakRef();
    return DPdfiumPage(strongRef, i);
}

QT_END_NAMESPACE

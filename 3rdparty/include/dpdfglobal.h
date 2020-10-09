#ifndef DPDFGLOBAL_H
#define DPDFGLOBAL_H

#include <QtCore/qglobal.h>

#ifndef QT_STATIC
#    if defined(BUILD_DEEPIN_PDFIUM_LIB)
#      define DEEPIN_PDFIUM_EXPORT Q_DECL_EXPORT
#    else
#      define DEEPIN_PDFIUM_EXPORT Q_DECL_IMPORT
#    endif
#else
#    define DEEPIN_PDFium_EXPORT
#endif

class DPDFGlobal
{
public:
    DPDFGlobal();
    ~DPDFGlobal();

private:
    void initPdfium();
    void shutdownPdfium();
};

#endif // DPDFGLOBAL_H
